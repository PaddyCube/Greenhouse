#ifndef WMINTPARAMETER_H
#define WMINTPARAMETER_H

class IntParameter : public WiFiManagerParameter
{
public:
    IntParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10)
        : WiFiManagerParameter("")
    {
        init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
    }

    long getValue()
    {
        return String(WiFiManagerParameter::getValue()).toInt();
    }

    void setValue(int value)
    {
        WiFiManagerParameter::setValue(String(value).c_str(),10);
    }
};

#endif