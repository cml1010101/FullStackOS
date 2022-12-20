#include <JSON.h>
JSONNode::JSONNode()
{
    nodes = {};
    nodes.setCompare(strcmp);
    strings = {};
    strings.setCompare(strcmp);
    ints = {};
    ints.setCompare(strcmp);
    nodeLists = {};
    nodeLists.setCompare(strcmp);
    stringLists = {};
    stringLists.setCompare(strcmp);
    intLists = {};
    intLists.setCompare(strcmp);
}
template<>
const char* JSONNode::getProperty<const char*>(const char* propertyName) 
{
    return strings[propertyName];
}
template<>
int JSONNode::getProperty<int>(const char* propertyName) 
{
    return ints[propertyName];
}
template<>
JSONNode* JSONNode::getProperty<JSONNode*>(const char* propertyName) 
{
    return nodes[propertyName];
}
template<>
Vector<const char*> JSONNode::getProperty<Vector<const char*>>(const char* propertyName) 
{
    return stringLists[propertyName];
}
template<>
Vector<int> JSONNode::getProperty<Vector<int>>(const char* propertyName) 
{
    return intLists[propertyName];
}
template<>
Vector<JSONNode*> JSONNode::getProperty<Vector<JSONNode*>>(const char* propertyName) 
{
    return nodeLists[propertyName];
}
template<>
void JSONNode::setProperty<int>(const char* propertyName, int i)
{
    if (!ints.has(propertyName)) ints.push(propertyName, i);
    else ints[propertyName] = i;
}
template<>
void JSONNode::setProperty<const char*>(const char* propertyName, const char* i)
{
    if (!strings.has(propertyName))
    {
        strings.push(propertyName, i);
    }
    else strings[propertyName] = i;
}
template<>
void JSONNode::setProperty<JSONNode*>(const char* propertyName, JSONNode* i)
{
    if (!nodes.has(propertyName)) nodes.push(propertyName, i);
    else nodes[propertyName] = i;
}
const char* JSONNode::toString()
{
    const char* out = "{";
    bool first = true;
    for (size_t i = 0; i < strings.size(); i++)
    {
        if (!first) out = strcat(out, ",");
        out = strcat(out, "\"");
        out = strcat(strcat(out, strings.getPair(i).key), "\":\"");
        out = strcat(strcat(out, strings.getPair(i).val), "\"");
        first = false;
    }
    for (size_t i = 0; i < ints.size(); i++)
    {
        if (!first) out = strcat(out, ",");
        out = strcat(out, "\"");
        out = strcat(strcat(out, ints.getPair(i).key), "\":");
        out = strcat(out, itoa(ints.getPair(i).val, 10));
        first = false;
    }
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (!first) out = strcat(out, ",");
        out = strcat(out, "\"");
        out = strcat(strcat(out, nodes.getPair(i).key), "\":");
        out = strcat(out, nodes.getPair(i).val->toString());
        first = false;
    }
    for (size_t i = 0; i < stringLists.size(); i++)
    {
        if (!first) out = strcat(out, ",");
        out = strcat(out, "\"");
        out = strcat(strcat(out, stringLists.getPair(i).key), "\":[");
        for (size_t j = 0; j < stringLists.getPair(i).val.size(); j++)
        {
            out = strcat(strcat(strcat(out, "\""), stringLists.getPair(i).val[i]), "\"");
            if (i != (stringLists.getPair(i).val.size() - 1)) out = strcat(out, ",");
        }
        out = strcat(out, "]");
        first = false;
    }
    for (size_t i = 0; i < intLists.size(); i++)
    {
        if (!first) out = strcat(out, ",");
        out = strcat(out, "\"");
        out = strcat(strcat(out, intLists.getPair(i).key), "\":[");
        for (size_t j = 0; j < intLists.getPair(i).val.size(); j++)
        {
            out = strcat(out, itoa(intLists.getPair(i).val[i], 10));
            if (i != (intLists.getPair(i).val.size() - 1)) out = strcat(out, ",");
        }
        out = strcat(out, "]");
        first = false;
    }
    for (size_t i = 0; i < nodeLists.size(); i++)
    {
        if (!first) out = strcat(out, ",");
        out = strcat(out, "\"");
        out = strcat(strcat(out, nodeLists.getPair(i).key), "\":[");
        for (size_t j = 0; j < nodeLists.getPair(i).val.size(); j++)
        {
            out = strcat(out, nodeLists.getPair(i).val[i]->toString());
            if (i != (nodeLists.getPair(i).val.size() - 1)) out = strcat(out, ",");
        }
        out = strcat(out, "]");
        first = false;
    }
    out = strcat(out, "}");
    return out;
}
JSONNode parseJSON(const char* data)
{
    bool inList = false, first = true, inString = false, inKey = true, inInt = false;
    char lastC = 0;
    const char* currentProperty = "", *currentValueString = "";
    int currentInt = 0;
    size_t k = 0;
    JSONNode node;
    while (true)
    {
        char c = data[k++];
        if (inString && c != '\\')
        {
            if (c == '\"' && lastC != '\\')
            {
                inString = false;
                if (!inKey)
                {
                    if (inList)
                    {
                        if (!node.stringLists.has(currentProperty))
                        {
                            node.stringLists.push(currentProperty, {});
                        }
                        node.stringLists[currentProperty].push(currentValueString);
                    }
                    else
                    {
                        node.strings.push(currentProperty, currentValueString);
                    }
                }
            }
            else if (inKey) currentProperty = strapp(currentProperty, c);
            else currentValueString = strapp(currentValueString, c);
        }
        else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
        else if (c >= '0' && c <= '9')
        {
            currentInt = currentInt * 10 + (int)(c - '0');
            inInt = true;
        }
        else if (c == ':')
        {
            inKey = false;
        }
        else if (c == ',')
        {
            if (inList)
            {
                currentValueString = "";
                if (inInt)
                {
                    if (!node.intLists.has(currentProperty))
                    {
                        node.intLists.push(currentProperty, {});
                    }
                    node.intLists[currentProperty].push(currentInt);
                }
                currentInt = 0;
            }
            else
            {
                inKey = true;
                if (inInt)
                {
                    node.ints[currentProperty] = currentInt;
                }
                inInt = false;
                currentProperty = "";
                currentValueString = "";
                currentInt = 0;
            }
        }
        else if (!first && c == '{')
        {
            JSONNode* newNode = new JSONNode;
            *newNode = parseJSON(data + k);
            if (inList)
            {
                if (!node.nodeLists.has(currentProperty))
                {
                    node.nodeLists.push(currentProperty, {});
                }
                node.nodeLists[currentProperty].push(newNode);
            }
            else
            {
                node.nodes.push(currentProperty, newNode);
            }
        }
        else if (c == '[')
        {
            inList = true;
        }
        else if (c == ']')
        {
            if (inInt)
            {
                if (!node.intLists.has(currentProperty))
                {
                    node.intLists.push(currentProperty, {});
                }
                node.intLists[currentProperty].push(currentInt);
            }
            inList = false;
        }
        else if (c == '\"')
        {
            inString = true;
        }
        else if (c == '}')
        {
            if (inList)
            {
                currentValueString = "";
                if (inInt)
                {
                    if (!node.intLists.has(currentProperty))
                    {
                        node.intLists.push(currentProperty, {});
                    }
                    node.intLists[currentProperty].push(currentInt);
                }
                currentInt = 0;
            }
            else
            {
                inKey = true;
                if (inInt)
                {
                    node.ints[currentProperty] = currentInt;
                }
                inInt = false;
                currentProperty = "";
                currentValueString = "";
                currentInt = 0;
            }
            break;
        }
        lastC = c;
        first = false;
    }
    return node;
}