#include <iostream>
#include <vector>
#include "agent.hpp"
#include "mmanager.hpp"
#include "stochastic_decorator.hpp"
#include "simple_decorator.hpp"

#include "tinyxml2.h"

#include "special_agent.hpp"

/*
 * Q. 클라이언트 코드에 왜 클래스 타입을 몰라도 동작하는지
 * A. oop의 개념 중 하나인 다형성으로 상속된 부모 클래스가 같을 때
 *    클라이언트 코드는 동일한 인터페이스를 공유하는 메서드를 호출하면
 *    구체적인 객체 타입에 관계없이 생성된 객체에 따라 자신의 메서드를 실행함   
 */
class InitManager
{
public:
    InitManager() {}
    // 생성자를 통해 path를 인자값으로 전달
    InitManager(std::string path)
    {
        // tinyxml을 사용하여 인자값으로 주어진 경로의 파일을 load
        doc.LoadFile(path.c_str());

        // for문 바깥에서 선언하여 for문을 돌 때마다 생성되는 것을 방지함
        double x, y, heading, speed, drange;
        {
            //  최상위 scenario 안에 있는 AgentList 객체를 가져옴
            tinyxml2::XMLElement* agListElem = doc.FirstChildElement("scenario")->FirstChildElement("AgentList");
            // attribute로 정의된 x, y, heading, speed, drange를 가져옴
            for(tinyxml2::XMLElement* agElem = agListElem->FirstChildElement(); agElem != NULL; agElem = agElem->NextSiblingElement())
            {
                agElem->QueryDoubleAttribute( "x", &x );
                agElem->QueryDoubleAttribute( "y", &y );
                agElem->QueryDoubleAttribute( "heading", &heading );
                agElem->QueryDoubleAttribute( "speed", &speed );
                agElem->QueryDoubleAttribute( "drange", &drange );

                // xml 파일을 통해 가져온 변수로 Agent 클래스를 만들고 m_agentList의 push_back 함
                CAgent* ag = new CAgent(x, y, heading, speed, drange);
                m_agentList.push_back(ag);
            }
        }

        {
            // 위 과정과 동일하지만 SAgentList를 가져옴
            tinyxml2::XMLElement* agListElem = doc.FirstChildElement("scenario")->FirstChildElement("SAgentList");
            
            for(tinyxml2::XMLElement* agElem = agListElem->FirstChildElement(); agElem != NULL; agElem = agElem->NextSiblingElement())
            {
                agElem->QueryDoubleAttribute( "x", &x );
                agElem->QueryDoubleAttribute( "y", &y );
                agElem->QueryDoubleAttribute( "heading", &heading );
                agElem->QueryDoubleAttribute( "speed", &speed );
                agElem->QueryDoubleAttribute( "drange", &drange );

                CAgent* ag = new CSpecialAgent(x, y, heading, speed, drange);
                m_agentList.push_back(ag);
            }
        }
    }

    // &로 vector 자체를 반환
    std::vector<CAgent*>& get_agent_list()
    {
        return m_agentList;
    }

private:
    // 멤버 변수로 tinyxml의 클래스와 agent를 저장해둘 vector를 가짐
    tinyxml2::XMLDocument doc;
    std::vector<CAgent*> m_agentList;

};

int main(int argc, char** argv)
{// Clinet Code  

    // 인자값으로 test.xml을 넘겨 test.xml을 읽고 정의된 agent 데이터를 통해 객체를 생성하고 vector에 저장
    InitManager init_manager("test.xml");

    /* Simulation Engine */
    CManeuverManager mmanager;

    // init_manager에 저장된 agent list를 가져와서 list 안에 있는 모든 agent를 publisher로 등록
    for(std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        iter != init_manager.get_agent_list().end(); iter++)
    {
        mmanager.register_publisher(*iter);
    }

    /* Simulation Model Registration */

    // CAgent* p_agent1 = new CAgent(10,  0, 0, 1, 10);
    // p_agent1 = new StochasticDecorator(p_agent1);
    // p_agent1 = new SimpleDecorator(p_agent1);
    
    // mmanager.register_publisher(p_agent1);

    // CAgent* p_agent2 = new CAgent(10,  30, 180, 1, 10);
    // mmanager.register_publisher(p_agent2);
    
    /* Simulation Engine Initialization */

    double sim_time;
    double time_step = 1;

    for(sim_time = 0; sim_time < 30; sim_time += time_step)
    {   
        //p_agent1->maneuver(time_step);
        //p_agent2->maneuver(time_step);

        //p_agent1->detect(p_agent2);
        //p_agent2->detect(p_agent1);

        mmanager.svc(time_step);

        std::cout << "----" << std::endl;
        for(std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
            iter != init_manager.get_agent_list().end(); iter++)
        {
            std::cout << "Time: " << sim_time << ", " << **iter << std::endl;
            // (*iter) -> CAgent*
            // *(*iter) -> CAgent
        }
    }
    return 0;
}
