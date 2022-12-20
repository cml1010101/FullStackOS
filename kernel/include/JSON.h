#ifndef JSON_H
#define JSON_H
#include <SmartOS.h>
#ifdef __cplusplus
class JSONNode
{
public:
    Map<const char*, JSONNode*> nodes;
    Map<const char*, const char*> strings;
    Map<const char*, int> ints;
    Map<const char*, Vector<JSONNode*>> nodeLists;
    Map<const char*, Vector<const char*>> stringLists;
    Map<const char*, Vector<int>> intLists;
    JSONNode();
    template<class T>
    T getProperty(const char* propertyName);
    template<class T>
    void setProperty(const char* propertyName, T t);
    const char* toString();
};
JSONNode parseJSON(const char* data);
#endif
#endif