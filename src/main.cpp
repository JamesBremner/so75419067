#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cGraphData.h"
#include "GraphTheory.h"

class cProblem
{
public:
    cGraphData g;
    std::string start, end;
    std::vector<std::string> cover;

    std::vector<std::vector<int>> vpath;

    std::vector<std::vector<std::vector<int>>> vVDP;

    bool paths();
    void collect();
    void display();
};

std::vector<std::string>
tokenize(
    const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

void read(
    cProblem &P,
    const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("No input");

    P.g.directed(true);

    std::string stype, sn1, sn2;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'l':
            ifs >> sn1 >> sn2;
            P.g.findorAdd(sn1, sn2, "1");
            break;
        case 's':
            ifs >> sn1;
            P.start = sn1;
            break;
        case 't':
            ifs >> sn1;
            P.end = sn1;
            break;
        case 'c':
            ifs >> sn1;
            P.cover.push_back(sn1);
            break;
        }

        ifs >> stype;
    }
}

bool cProblem::paths()
{
    for (auto &c : cover)
    {
        std::vector<int> apath;
        apath = path(g, start, c);
        if (!apath.size())
            return false;
        apath.pop_back();
        auto p2 = path(g, c, end);
        if (!p2.size())
            return false;
        apath.insert(apath.end(), p2.begin(), p2.end());
        vpath.push_back(apath);
    }
    return true;
}

void cProblem::collect()
{
    // loop over paths
    for (auto &P : vpath)
    {
        // loop over collections
        bool found = false;
        for (auto &cs : vVDP)
        {
            //loop over paths in collection
            bool disjoint;
            for (auto &csPath : cs)
            {
                // check that P is vertex disjoint with path in collection
                disjoint = true;
                for (auto vc : csPath)
                {
                    for (auto &vp : P)
                    {
                        if (vp == vc)
                            disjoint = false;
                    }
                }
            }

            if (disjoint)
            {
                // P is vertex disjoint from every path in collection
                // add P to the collection
                cs.push_back(P);
                found = true;
                break;
            }
        }
        if (!found)
        {
            // P was NOT vertex disjoint with the paths in any collection
            // start a new collection with P
            std::vector<std::vector<int>> collection;
            collection.push_back(P);
            vVDP.push_back(collection);
        }
    }
}
void cProblem::display()
{
    for (auto &p : vpath)
    {
        for (auto &v : p)
        {
            std::cout << g.userName(v) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nCollections\n";
    for (auto &collection : vVDP)
    {
        for (auto &p : collection)
        {
            for (auto &v : p)
            {
                std::cout << g.userName(v) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

main()
{
    cProblem theProblem;
    read(theProblem, "../data/data.txt");
    if( ! theProblem.paths() )
    {
        std::cout << "no feasible paths to or from a cover vertex\n";
        return 1;
    }
    theProblem.collect();
    theProblem.display();

    return 0;
}
