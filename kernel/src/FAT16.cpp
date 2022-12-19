#include <FAT16.h>
uint16_t FAT16::readFAT(uint16_t entry)
{
    uint64_t sector = fatOffset + (uint64_t)entry / 256;
    uint16_t* entries = new uint16_t[256];
    device->readSectors(sector, entries, 1);
    return entries[entry % 256];
}
void FAT16::writeFAT(uint16_t idx, uint16_t entry)
{
    uint64_t sector = fatOffset + (uint64_t)idx / 256;
    uint16_t entries[256];
    device->readSectors(sector, entries, 1);
    entries[idx % sectorsPerFAT] = entry;
    device->writeSectors(sector, entries, 1);
}
void FAT16::readCluster(uint16_t idx, void* buffer)
{
    if (idx == 0x1)
    {
        device->readSectors(rootOffset + (uint64_t)idx * sectorsPerCluster, buffer, sectorsPerCluster);
        return;
    }
    device->readSectors(dataOffset + (uint64_t)idx * sectorsPerCluster - 1, buffer,
        sectorsPerCluster);
}
void FAT16::writeCluster(uint16_t idx, const void* buffer)
{
    if (idx == 0x1)
    {
        device->writeSectors(rootOffset + (uint64_t)idx * sectorsPerCluster, buffer,
            sectorsPerCluster);
        return;
    }
    device->writeSectors(dataOffset + (uint64_t)idx * sectorsPerCluster - 1, buffer,
        sectorsPerCluster);
}
void filenameToFAT(char* fatname, const char* name)
{
    size_t i = 0;
    size_t j = 0;
    memset(fatname, ' ', 11);
    while (name[j] && i < 8)
    {
        if (name[j] != '.')
        {
            fatname[i] = (name[j] <= 'z' && name[j] >= 'a') ? name[j] + ('A' - 'a') : name[j];
            j++;
        }
        i++;
    }
    if (!name[j]) return;
    j++;
    while (name[j] && i < 11)
    {
        fatname[i++] = (name[j] <= 'z' && name[j] >= 'a') ? name[j] + ('A' - 'a') : name[j];
        j++;
    }
}
void fatToFilename(char* name, const char* fatname)
{
    size_t i = 0;
    size_t j = 0;
    while (i < 8)
    {
        if (fatname[i] != ' ')
        {
            name[j] = fatname[i];
            j++;
        }
        i++;
    }
    if (fatname[i] == ' ') goto skip;
    name[j] = '.';
    j++;
    while (i < 11)
    {
        if (fatname[i] != ' ')
        {
            name[j] = fatname[i];
            j++;
        }
        i++;
    }
skip:
    name[j] = 0;
}
bool FAT16::exists(const char* path)
{
    if (path[0] == '/') path++;
    uint64_t cluster = 0x1;
    size_t i = 0, j = 0;
    while (path[i])
    {
        if (path[i] == '/')
        {
            char fatFilename[11];
            char filename[13];
            filename[12] = 0;
            memcpy(filename, &path[j], i - j);
            memset(filename + (i - j), 0, 11 - (i - j));
            filenameToFAT(fatFilename, filename);
            cluster = searchDirectory(cluster, fatFilename, NULL);
            if (cluster >= 0xFFF7) return false;
            j = i + 1;
        }
        i++;
    }
    char fatFilename[12];
    fatFilename[11] = 0;
    char filename[13];
    filename[12] = 0;
    memcpy(filename, &path[j], i - j);
    memset(filename + (i - j), 0, 11 - (i - j));
    filenameToFAT(fatFilename, filename);
    cluster = searchDirectory(cluster, fatFilename, NULL);
    return cluster < 0xFFF7;
}
void FAT16::read(File* file, void* dest, size_t size)
{
    void* tmp = malloc(512);
    size_t startCluster = *(uint32_t*)file->metadata;
    size_t clusterIdx = 0;
    while (file->position > ((clusterIdx + 1) * sectorsPerCluster * 512))
    {
        startCluster = readFAT(startCluster);
        clusterIdx++;
    }
    size_t endPos = size + file->position;
    size_t endCluster = startCluster;
    while (endPos > ((clusterIdx + 1) * sectorsPerCluster * 512))
    {
        endCluster = readFAT(endCluster);
        clusterIdx++;
    }
    size_t readPos = file->position;
    size_t writePos = 0;
    tmp = malloc(512);
    uint8_t* clusterBuffer = new uint8_t[512 * sectorsPerCluster];
    while (startCluster != endCluster)
    {
        readCluster(startCluster, clusterBuffer);
        memcpy(dest + writePos, clusterBuffer + (readPos % (sectorsPerCluster * 512)),
            (sectorsPerCluster * 512) - (readPos % (sectorsPerCluster * 512)));
        writePos += (sectorsPerCluster * 512) - (readPos % (sectorsPerCluster * 512));
        readPos += (sectorsPerCluster * 512) - (readPos % (sectorsPerCluster * 512));
        startCluster = readFAT(startCluster);
    }
    readCluster(startCluster, clusterBuffer);
    memcpy(dest + writePos, clusterBuffer + (readPos % (sectorsPerCluster * 512)),
        endPos - readPos);
    free(clusterBuffer);
}
void FAT16::write(File* file, const void* src, size_t size)
{
    size_t startCluster = *(uint32_t*)file->metadata;
    size_t clusterIdx = 0;
    while (file->position >= ((clusterIdx + 1) * sectorsPerCluster * 512))
    {
        startCluster = readFAT(startCluster);
    }
    size_t endPos = size + file->position;
    size_t endCluster = startCluster;
    while (endPos > ((clusterIdx + 1) * sectorsPerCluster * 512))
    {
        endCluster = readFAT(endCluster);
    }
    size_t readPos = file->position;
    size_t writePos = 0;
    uint8_t* clusterBuffer = new uint8_t[512 * sectorsPerCluster];
    while (startCluster != endCluster)
    {
        readCluster(startCluster, clusterBuffer);
        memcpy(clusterBuffer + (readPos % (sectorsPerCluster * 512)), src + writePos,
            (sectorsPerCluster * 512) - (readPos % (sectorsPerCluster * 512)));
        writeCluster(startCluster, clusterBuffer);
        writePos += (readPos % (sectorsPerCluster * 512));
        readPos /= sectorsPerCluster * 512;
        readPos += 1;
        readPos *= sectorsPerCluster * 512;
        startCluster = readFAT(startCluster);
    }
    readCluster(startCluster, clusterBuffer);
    memcpy(clusterBuffer + (readPos % (sectorsPerCluster * 512)), src + writePos,
        endPos - readPos);
    writeCluster(startCluster, clusterBuffer);
}
uint16_t FAT16::searchDirectory(uint16_t cluster, const char* fatname, size_t* size)
{
    DirectoryEntry* entries = (DirectoryEntry*)malloc(16 * sectorsPerCluster * sizeof(DirectoryEntry));
    while (cluster < 0xFFF7)
    {
        readCluster(cluster, entries);
        for (size_t i = 0; i < 16 * sectorsPerCluster; i++)
        {
            if (entries[i].name[0] == 0) return 0xFFF8;
            if (entries[i].name[0] == 0xE5) continue;
            if (memcmp(entries[i].name, fatname, 11) == 0)
            {
                if (size) *size = entries[i].size;
                return entries[i].clusterLow;
            }
        }
        cluster = readFAT(cluster);
    }
    return 0xFFF8;
}
File* FAT16::open(const char* path)
{
    if (path[0] == '/') path++;
    uint64_t cluster = 0x1;
    size_t i = 0, j = 0;
    size_t lastSize;
    while (path[i])
    {
        if (path[i] == '/')
        {
            char fatFilename[11];
            char filename[13];
            filename[12] = 0;
            memcpy(filename, &path[j], i - j);
            memset(filename + (i - j), 0, 11 - (i - j));
            filenameToFAT(fatFilename, filename);
            cluster = searchDirectory(cluster, fatFilename, &lastSize);
            j = i + 1;
        }
        i++;
    }
    char fatFilename[12];
    fatFilename[11] = 0;
    char filename[13];
    filename[12] = 0;
    memcpy(filename, &path[j], i - j);
    memset(filename + (i - j), 0, 11 - (i - j));
    filenameToFAT(fatFilename, filename);
    cluster = searchDirectory(cluster, fatFilename, &lastSize);
    uint32_t* clusterPtr = new uint32_t;
    *clusterPtr = (uint32_t)cluster;
    return new File(this, clusterPtr, lastSize, path);
}
FAT16::FAT16(StorageDevice* dev, size_t offset)
{
    device = dev;
    partitionOffset = offset;
    FAT16BootBlock block;
    dev->readSectors(partitionOffset, &block, 1);
    sectorsPerCluster = block.bpb.sectorsPerCluster;
    sectorsPerFAT = block.bpb.smallSectorsPerFAT;
    fatOffset = (uint64_t)block.bpb.reservedSectors + partitionOffset;
    dataOffset = (uint64_t)block.bpb.numberOfFAT * sectorsPerFAT + partitionOffset
        + (block.bpb.numberOfRootEntries + 15) / 16;
    rootOffset = (uint64_t)block.bpb.numberOfFAT * sectorsPerFAT + partitionOffset;
}
const char* FAT16::getName()
{
    return "FAT16";
}
Vector<File*> FAT16::list(const char* path)
{
    if (path[0] == '/') path++;
    uint64_t cluster = 0x1;
    size_t i = 0, j = 0;
    DirectoryEntry* entries = new DirectoryEntry[32 * sectorsPerCluster];
    size_t lastSize;
    while (path[i])
    {
        if (path[i] == '/')
        {
            char fatFilename[11];
            char filename[13];
            filename[12] = 0;
            memcpy(filename, &path[j], i - j);
            memset(filename + (i - j), 0, 11 - (i - j));
            filenameToFAT(fatFilename, filename);
            cluster = searchDirectory(cluster, fatFilename, &lastSize);
            j = i + 1;
        }
        i++;
    }
    Vector<File*> files = Vector<File*>();
    while (cluster < 0xFFF7)
    {
        readCluster(cluster, entries);
        for (size_t i = 0; i < 32 * sectorsPerCluster; i++)
        {
            if (entries[i].name[0] == 0) break;
            if (entries[i].name[0] == 0xE5) continue;
            char fname[12];
            fatToFilename(fname, entries[i].name);
            char* newPath = new char[strlen(path) + strlen(fname) + 2];
            memcpy(newPath, path, strlen(path));
            newPath[strlen(path)] = '/';
            memcpy(newPath + strlen(path), fname, strlen(fname) + 1);
            uint32_t* clusterPtr = new uint32_t;
            *clusterPtr = cluster;
            files.push(new File(this, clusterPtr, entries[i].size, newPath));
        }
        cluster = readFAT(cluster);
    }
    return files;
}