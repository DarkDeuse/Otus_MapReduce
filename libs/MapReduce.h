#include <array>
#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <stdlib.h>
#include <thread>
#include <vector>

#include "queue.h"

class MapReduce
{
    
    public:
        typedef std::iostream::pos_type  pos_t;
        typedef std::vector<std::string> vString_t;
        typedef std::vector<vString_t> v2String_t;
        typedef tsVector<tsVector<std::string>> v2TsvString;

        MapReduce() = delete;
        MapReduce(std::string&, size_t&, size_t&);

        bool setMapper(void (*_mapper)(std::ifstream&, pos_t&, pos_t&, vString_t&, std::mutex& ));
        bool setReducer(void (*_reducer)(vString_t&, size_t& ));
        void start();

        
    private:
        
        void map();
        void reduce();

        void (*m_mapperThread)(std::ifstream& fileIn, pos_t&, pos_t&, vString_t&, std::mutex&); 
        void mapperThread(pos_t, pos_t, vString_t&);

        void (*m_reducerThread)(vString_t&,  size_t& );
        void reducerThread(size_t ) const;
            
        size_t getHash(const std::string&) const;

        template <typename T>
        bool setFunction(T&, T&);

        static const std::string hasher;

        v2String_t                              m_MapperResults;
        v2TsvString                             m_ReducerResults;
        std::string                             m_filename;
        std::ifstream                           m_inputFile;
        mutable std::mutex                      m_mutex; 
        mutable std::condition_variable         m_cv;
        size_t                                  m_mapNum, m_redNum;
};

template <typename T>
        bool MapReduce::setFunction(T& f1, T& f2)
{
    
    try
    {
        if (f2){
            f1 = f2;
            return true;
        }
        else
            std::cerr << "Error: nullptr pointer" << '\n';

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
    (void)f1;
}