#include "graphics.hpp"
#include<vector>
#include <fstream>
#include <iostream>
#include <utility> //pairhez kell
using namespace genv;
using namespace std;
struct terkep
{
private:
    unsigned XX, YY ;
    vector<vector<int>> geodata;
    vector <vector <bool>> bejart_helyek_viz;
    vector <vector <bool>> bejart_helyek_fold;

public:
    terkep (const string & filename)
    {
        ifstream f(filename);
        f>>XX>>YY;
        gout.open(XX,YY);
        geodata = vector<vector<int>> (XX, vector<int> (YY, 0)) ;

        for(size_t x = 0; x< XX; x++)
        {
            for(size_t y= 0 ; y < YY; y ++)
            {
                f>>geodata[x][y];
            }
        }

        f.close();

    }
    void terkep_rajzolo ()
    {
        for(size_t x = 0; x< XX; x++)
        {
            for(size_t y= 0 ; y < YY; y ++)
            {
                if(geodata[x][y] < 0) gout << move_to(x,y) << color(50 + 0.3 * geodata[x][y], 50 + 0.3 * geodata[x][y], 200 + geodata[x][y]) << dot;
                else gout << move_to(x,y) << color(2 * geodata[x][y], 125 + geodata[x][y], 0) << dot;
            }
        }
        gout<<refresh;
    }
    void hibrid_bejaras_fold (unsigned & posx, unsigned & posy)
    {
        bejart_helyek_fold=vector <vector <bool>> (XX, vector<bool> (YY, false));
        vector< pair< unsigned, unsigned> > koord_halmaz; //elso x, masodik y
        pair<unsigned, unsigned> kezdeti_koord = make_pair (posx, posy);
        koord_halmaz.push_back(kezdeti_koord);
        while (!koord_halmaz.empty())
        {
            pair<unsigned, unsigned> koord = koord_halmaz.front(); // elso elemet kimenti
                koord_halmaz.erase(koord_halmaz.begin()); // aztan kitörli
            if(koord.first < 0 || koord.first > XX-1 ||
               koord.second < 0 || koord.second > YY-1 ||
               (koord.first- posx)*(koord.first - posx)+(koord.second-posy)*(koord.second-posy) > 50*50 ||
               geodata[koord.first][koord.second] >= 20 ||
               bejart_helyek_fold[koord.first][koord.second] == true ) // ha a feltetelek kozul vmelyik teljesul, ujraindul a ciklus a kovi szomszeddal
            {
                continue;
            }
            bejart_helyek_fold[koord.first][koord.second] = true ;
            if (geodata[koord.first][koord.second]>0)
            {
                gout << move_to(koord.first, koord.second) <<color (255,255,255)<<dot;
                koord_halmaz.push_back (make_pair(koord.first + 1, koord.second));
            koord_halmaz.push_back (make_pair(koord.first - 1, koord.second));
            koord_halmaz.push_back (make_pair(koord.first , koord.second + 1));
            koord_halmaz.push_back (make_pair(koord.first , koord.second - 1));
            }
            else if (geodata[koord.first][koord.second]<0)
            {
                koord_halmaz.push_back (make_pair(koord.first + 1, koord.second));
            koord_halmaz.push_back (make_pair(koord.first - 1, koord.second));
            koord_halmaz.push_back (make_pair(koord.first , koord.second + 1));
            koord_halmaz.push_back (make_pair(koord.first , koord.second - 1));
            continue;
            }

            gout<<refresh;
        }


    }
    void hibrid_bejaras_viz (const unsigned & pos_x, const unsigned & pos_y)
    {
        terkep_rajzolo();
        bejart_helyek_viz=vector <vector <bool>> (XX, vector<bool> (YY, false));
        vector< pair< unsigned, unsigned> > koord_halmaz; //elso x, masodik y
        pair<unsigned, unsigned> kezdeti_koord = make_pair (pos_x, pos_y);
        koord_halmaz.push_back(kezdeti_koord);
        while (!koord_halmaz.empty())
        {
            pair<unsigned, unsigned> koord = koord_halmaz.front(); // elso elemet kimenti
                koord_halmaz.erase(koord_halmaz.begin()); // aztan kitörli
            if(koord.first < 0 || koord.first > XX-1 ||
               koord.second < 0 || koord.second > YY-1 ||
               geodata[koord.first][koord.second] > -10 ||
               bejart_helyek_viz[koord.first][koord.second] == true ) // ha a feltetelek kozul vmelyik teljesul, ujraindul a ciklus a kovi szomszeddal
            {
                continue;
            }
            bejart_helyek_viz[koord.first][koord.second] = true ;
            gout << move_to(koord.first, koord.second) <<color (255,0,0)<<dot<<refresh;
            for (int dx = koord.first - 1 ; dx < koord.first + 1 ; dx++)
            {
                for (int dy = koord.second - 1 ;dy < koord.second + 1 ; dy ++)
                {
                    if ( geodata[dx][dy]> 0)
                    {
                        hibrid_bejaras_fold(koord.first, koord.second);
                    }
                }
            }

            koord_halmaz.push_back (make_pair(koord.first + 1, koord.second));
            koord_halmaz.push_back (make_pair(koord.first - 1, koord.second));
            koord_halmaz.push_back (make_pair(koord.first , koord.second + 1));
            koord_halmaz.push_back (make_pair(koord.first , koord.second - 1));

        }
    } // a ciklus vegere kiurul a vektor, nem fog ujraindulni

};
int main()
{
    terkep t("terkep.txt");
    t.terkep_rajzolo();
    event ev;
    std::cout<<"Pirossal van jelolve a tengeralattjaro mozgasa,"<<endl;
    std::cout<<"tehat azok az osszefuggo vizek, amik 10-nel melyebbek." <<endl;
    std::cout<<"Feherrel pedig azok a teruletek, ahova dron kuldheto minden tengeri pontbol."<<endl;
    while(gin >> ev && ev.keycode != key_escape)
    {
        if (ev.button == btn_left) t.hibrid_bejaras_viz(ev.pos_x, ev.pos_y);

    }
    return 0;
}
