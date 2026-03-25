#pragma once
#include "Start.h"

class OBJ {
public:
    std::string name;
    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;
    std::vector<std::vector<std::vector<long long>>> f;
};

class OBJParser {
private:
    OBJParser() {};
    ~OBJParser() {};
    OBJParser(OBJParser&&) = delete;
    const OBJParser& operator=(const OBJParser&) = delete;

    std::vector<std::string> splitStr(std::string str, char splitWord,int prefixLength)
    {
        str.erase(0, prefixLength);
        std::vector<std::string> res;
        size_t i = str.find(splitWord);
        while (i!=-1)
        {
            res.emplace_back(str.substr(0, i));
            if (i + 1 == str.size())
            {
                return res;
            }
            else
            {
                str.erase(0, i + 1);
            }
            i = str.find(splitWord);
        }
        res.emplace_back(str);
        return res;
    }

    void vParser(std::string str, OBJ& obj)
    {
        static int k = 0;
        if (k < 1)
        {
            std::cout << k++;
        }
        std::vector<std::string> v = splitStr(str, ' ', 2);
        glm::vec3 tmp = { stod(v[0]), stod(v[1]), stod(v[2]) };
        obj.v.emplace_back(tmp);
    }

    void vnParser(std::string str, OBJ& obj)
    {
        static int k = 1;
        if (k < 2)
        {
            std::cout << k++;
        }
        std::vector<std::string> vn = splitStr(str, ' ', 3);
        glm::vec3 tmp = { stod(vn[0]), stod(vn[1]), stod(vn[2]) };
        obj.vn.emplace_back(tmp);
    }

    void vtParser(std::string str, OBJ& obj)
    {
        static int k = 2;
        if (k < 3)
        {
            std::cout << k++;
        }
        std::vector<std::string> vt = splitStr(str, ' ', 3);
        glm::vec2 tmp = { stod(vt[0]), stod(vt[1]) };
        obj.vt.emplace_back(tmp);
    }

    void fParser(std::string str, OBJ& obj)
    {
        static int k = 3;
        if (k < 4)
        {
            std::cout << k++ << std::endl;
        }
        std::vector<std::string> facePointstr = splitStr(str, ' ', 2),tmp;
        std::vector<std::vector<long long>> facePoint;
        for (int i = 0; i < facePointstr.size(); i++)
        {
            tmp = splitStr(facePointstr[i], '/', 0);
            std::vector<long long> pointAttribute;
            for (int j = 0; j < tmp.size(); j++)
            {
                pointAttribute.emplace_back(stoll(tmp[j]));
            }
            facePoint.emplace_back(pointAttribute);
        }
        obj.f.emplace_back(facePoint);
    }
public:

    static OBJParser& Instance() {
        static OBJParser instance;
        return instance;
    }
    OBJ Parser(std::string filePath)
    {
        OBJ obj;
        std::ifstream objFile(filePath);
        //objFile.open(filePath, std::ios::in);
        std::string str;
        while (std::getline(objFile, str))
        {
            if (str.substr(0, 2).compare("v ") == 0)
            {
                vParser(str, obj);
            }
            else if (str.substr(0, 3).compare("vt ") == 0)
            {
                vtParser(str, obj);
            }
            else if (str.substr(0, 3).compare("vn ") == 0)
            {
                vnParser(str, obj);
            }
            else if (str.substr(0, 2).compare("f ") == 0)
            {
                fParser(str, obj);
            }
        }
        objFile.close();
        return obj;
    }
};