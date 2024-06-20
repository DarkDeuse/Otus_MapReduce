#include"MapReduce.h"

const std::string MapReduce::hasher = "abcdefghijlklmnopqrstvwxyz";

MapReduce::MapReduce(std::string& filename, size_t& _mapNum, size_t& _redNum) : m_filename{filename}, m_mapNum{_mapNum}, m_redNum{_redNum}
{
    m_MapperResults.reserve(_mapNum);
    for (size_t i = 0; i<_mapNum; ++i )
        m_MapperResults.emplace_back(10);

    m_ReducerResults.reserve(_redNum);
    for (size_t i = 0; i<_redNum; ++i )
        m_ReducerResults.getVector().emplace_back(10);
}

bool MapReduce::setMapper(void (*_mapper)(std::ifstream&, pos_t&, pos_t&, vString_t&, std::mutex&))
{return setFunction( m_mapperThread , _mapper);}

bool MapReduce::setReducer(void (*_reducer)(vString_t&,  size_t& ))
{return setFunction( m_reducerThread , _reducer);}

size_t MapReduce::getHash(const std::string& str ) const{ 
    const char& c = std::tolower(str[0]);
    double pos = static_cast<double>(MapReduce::hasher.find(c));
    
    if (pos != std::string::npos){
        double buf = pos / MapReduce::hasher.size();
        size_t s1 = buf * m_redNum;
        return s1;
    }
        
    return 0;
}

void MapReduce::reducerThread( size_t iter) const
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
	    m_cv.wait(lock);
    }

    (*m_reducerThread) (m_ReducerResults[iter].getVector() , iter);
}

void MapReduce::mapperThread(pos_t begin , pos_t end ,vString_t& thVector)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
	    m_cv.wait(lock);
    }
    (*m_mapperThread) (m_inputFile, begin, end, thVector, m_mutex);
    (void)begin;
    (void)end;
    std::sort(thVector.begin(), thVector.end());
    for (auto& str : thVector){
        m_ReducerResults[getHash(str)].push(str);     
    }
}

void MapReduce::start()
{
    if (m_mapperThread && m_reducerThread){
        map();
        reduce();
    }
    else
        std::cerr << "maper or reducer function not set\n";
}

void MapReduce::map()
{
        
    m_inputFile.open(m_filename);
    m_inputFile.seekg(0, std::ios_base::end);
    std::vector<std::thread> tasks;
    pos_t begin{0};
    size_t counter{0}; 
    char temp{0};
    pos_t endPos = m_inputFile.tellg();
    pos_t part = endPos / m_mapNum;

    for (pos_t pos = part; pos<endPos; pos += part)
    {
        m_inputFile.seekg(pos, std::ios_base::beg);
        while (m_inputFile.read(&temp,1) && temp!='\n' && !m_inputFile.fail()); 
        pos = m_inputFile.tellg();
        tasks.emplace_back(&MapReduce::mapperThread, std::ref(*this), std::ref(begin), std::ref(pos), std::ref(m_MapperResults[counter++]));
        begin = pos;
    }
    std::this_thread::sleep_for (std::chrono::microseconds(500));
    m_cv.notify_all();

    for (auto& task : tasks  )
        if (task.joinable())
            task.join();

    m_inputFile.close();
}

void MapReduce::reduce(){

    std::vector<std::thread> tasks{m_redNum};
    
    for (size_t reducerIter = 0; reducerIter<m_redNum; ++reducerIter)
        tasks.emplace_back(&MapReduce::reducerThread, std::ref(*this), reducerIter );  
    
    std::this_thread::sleep_for (std::chrono::microseconds(1000));
    m_cv.notify_all();

    for (auto& task : tasks  )
        if (task.joinable())
            task.join();

}