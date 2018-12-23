#include "cliqueholder.h"
#include "gadget.h"
#include <cstdio>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <iostream>
using namespace std;

extern VI lookup;
extern Graph g;

inline bool compv(int a, int b) {
    return lookup[a]<lookup[b];
}

CliqueHolder::CliqueHolder() {
}

void CliqueHolder::init(double _tau, bool _filter, FILE *_fout) {

    fout = _fout;
    cnt = sumsize = 0;
    filter = _filter;
    tau = _tau;
}

bool CliqueHolder::exactFilter(VI &C1, VI &C2, int t) {

    int f1 = C1.front(), e1 = C1.back();
    int f2 = C2.front(), e2 = C2.back();
    return min(e1,e2)-max(f1,f2)<=t || (C1*C2).size()<=t;
}


int setin(int first1,int last1,int first2, int last2,int &v,int &v2,VI &a,VI &b)
{
    int result=0;
    int z=1;
      while (first1!=last1 && first2!=last2)
      {
        if (a[first1]<b[first2]) ++first1;
        else if (b[first2]<a[first1]) ++first2;
        else {
            if(z)
            {
                v=first2;//��һ����ͬ��λ��
                v2=first1;
                z=0;
            }
          ++result;
          ++first1;
          ++first2;
        }
      }
  return result;
}

int taggg=0;
bool CliqueHolder::passFilter(VI &C) {

    int upovl = (int)floor(C.size()*getTau());

    multimap<int,int>::iterator itlow, itup, it;
    itlow= lv.begin();
    itup= lv.upper_bound(g.deg(D.front())-1);

    vector<multimap<int,int>::iterator> toerase;
    for (it=itlow; it!=itup; ++it)
        toerase.push_back(it);
    for (int i=0; i<toerase.size(); ++i)
        lv.erase(toerase[i]);
    toerase.clear();

    int f1 , e1 ;
    int f2 = C.front(), e2 = C.back();
    int des;
    int las=C.size(),lap,lav,lavv;
    bool pass=true;
    int ncomp=0;
    for (it=itup; it!= lv.end(); ++it) {
        int i= (*it).second;
        ++ncomp;
        VI &cli=cliques[i];
        int j=cli.size()-1;
        while (j>=0 && lookup[C.front()] < lookup[cli[j]]) --j;
        if (cli.size()-j<3) {
                toerase.push_back(it);
                continue;
        }

        if (cliques[i].size()<upovl)
            continue;

        VI P=cli;
         sort(P.begin(), P.end());
        des=setin(upovl,las,upovl,P.size(),lav,lavv,C,P);//��׺ȡ������lav�ǵ�һ��λ��
        int we,geee;
        if(des)
        {
            f1=P.front();e1=P.back();
            we=setin(0,lavv,0,lav,lavv,geee,C,P);
            if (!(min(e1,e2)-max(f1,f2)<=upovl ||we+des<=upovl))//ǰ׺ȡ����
            {
                pass=false;
                break;
            }
        }
    }

    for (int i=0; i<toerase.size(); ++i)
        lv.erase(toerase[i]);

    return pass;
}


bool CliqueHolder::insert(VI &C) {

    // pass size threshold
    if (C.size()<ls)
        return false;

    // not covered by last
    int upovl = (int)floor(C.size()*getTau());
    if ((L*C).size()>upovl)
        return false;

    D=C;
    sort(D.begin(), D.end(), compv);

    if (filter) {
        if (!passFilter(C))
            return false;
    }

    int id=cliques.size();
    cliques.push_back(D);
    lv.insert(make_pair(g.deg(D.back()),id));

    sumsize += C.size();
    L = C;
    if (C.size()> maxcli.size())
        maxcli = C;

    vprint(D, fout);
    return true;
}

int CliqueHolder::updateScore(int i, VI &D) {

    // a.k.a. max spanning tree
    VI &C = cliques[i];
    C= C-D;
    return C.size();
}

int CliqueHolder::topk(int k) {

    printf("top-%d diverse cliques\n\n");
    int quality=0;
    VI best;
    int nc=cliques.size();
    vector<bool> inans(nc, 0);
    scores.resize(nc, -1);
    for (int i=0; i<min(k,nc); ++i) {
        int scorebest=-1, ibest=-1;
        for (int j=0; j<nc; ++j)
            if (!inans[j]) {
                int score = updateScore(j, best); // last best
                if (score>scorebest) {
                    scorebest = score;
                    ibest = j;
                }
            }
        quality+= scorebest;
        assert(scorebest>=0 && ibest>=0);
        best = cliques[ibest];
        inans[ibest]=1;
        vprint(best);
        printf("\n");
    }

    return quality;
}
