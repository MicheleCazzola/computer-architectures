#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <cassert>

#define LATO 7
#define MURI 16
#define CENTRI 2 * (LATO - 1) * (LATO - 1)
#define VERT 0
#define HORIZ 1

using namespace std;

typedef struct{
    int dir;
    int xc;
    int yc;
} Muro;

typedef struct{
    int xp;
    int yp;
} Player;

void random_comb(int *posizioni, int *muri, int n, int k, map<int,int> &occ, vector<int> &sol, int *globalMax, int *c, Player *start);
int comb(int pos, int *val, int *sol, int n, int k, int start, int cnt, int *globalMax, int *globalSol, int *globalSolLen, int *c, Player *st);
bool checkVal(int *muri, int n);
bool overlap(Muro a, Muro b);
bool checkRagg(int *muri, int n, vector<vector<bool>> ragg, int *maxQueueDim, Player *sol);
bool checkReachability(Player p, vector<Muro> muri, int yEnd, vector<vector<Player>> distance,int *maxQueueDim, Player *dest);
bool validPos(Player a, Player b, vector<Muro> muri);


int main() {

    Player st;
    int maxQueueDim, cnt, lenSol;
    int posizioni[CENTRI];
    int muri[MURI];
    int muriFinali[MURI];
    vector<int> sol;

    for(int i = 0; i < CENTRI; i++){
        posizioni[i] = i;
    }
    maxQueueDim = cnt = lenSol = 0;
    map<int, int> occ;
    comb(0, posizioni, muri, CENTRI, MURI, 0, 0, &maxQueueDim, muriFinali, &lenSol, &cnt, &st);

    printf("Len: %d\n", lenSol);
    for(int i = 0; i < lenSol; i++){
        printf("%d ", muriFinali[i]);
    }

    //random_comb(posizioni, muri, CENTRI, MURI, occ, sol, &maxQueueDim, &cnt, &st);

    printf("%d:\t", maxQueueDim);
    for(int m : sol){
        printf("%d %d %d\t", m % 2, (m / 2) % (LATO-1), (m / 2) / (LATO-1));
    }
    printf("\t%d %d", st.xp, st.yp);
    printf("\n");
    for(pair<int,int> p : occ){
        printf("%d: %d\n", p.first, p.second);
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

void random_comb(int *posizioni, int *muri, int n, int k, map<int,int> &occ, vector<int> &sol, int *globalMax, int *c, Player *start){

    const int MAX_ITER = 10000;
    int localMax, cnt;

    list<int> remPos(0);

    cnt = 0;
    while(cnt++ < MAX_ITER) {

        for(int i = 0; i < n; i++){
            remPos.push_back(posizioni[i]);
        }

        for (int i = 0; i < k/4; i++) {
            int p;
            do {
                p = rand() % n;
                muri[i] = posizioni[p];
            } while(!checkVal(muri, i+1));

            remPos.remove(posizioni[p]);

            vector<vector<bool>> ragg(LATO, vector<bool>(LATO));

            for (int x = 0; x < LATO; x++) {
                for (int y = 0; y < LATO; y++)
                    ragg[x][y] = true;
            }

            (*c)++;
            localMax = 0;
            checkRagg(muri, i+1, ragg, &localMax, start);
            occ[localMax]++;

            if(localMax > *globalMax){
                *globalMax = localMax;
                sol.resize(0);
                for(int j = 0; j < i+1; j++)
                    sol.push_back(muri[j]);
            }
            /*
            printf("%d: %d %d\t", *c, localMax, *globalMax);

            for(int j = 0; j < i+1; j++)
                printf("%d ", muri[j]);
            printf("\n");
             */
        }

        remPos.resize(0);
    }
}

int comb(int pos, int *val, int *sol, int n, int k, int start, int cnt, int *globalMax, int *globalSol, int *globalSolLen, int *c, Player *st) {
    int i, localMax;


    if(!checkVal(sol, pos)){
        return cnt;
    }

    vector<vector<bool>> ragg(LATO, vector<bool>(LATO));
    for(i = 0; i < LATO; i++){
        for(int j = 0; j < LATO; j++)
            ragg[i][j] = true;
    }

    (*c)++;
    localMax = 0;
    checkRagg(sol, pos, ragg, &localMax, st);
    *globalMax = max(*globalMax, localMax);

    if(*globalMax == localMax){
        for(i = 0; i < pos; i++){
            globalSol[i] = sol[i];
        }
        *globalSolLen = pos;
    }


    if (pos >= k/4) {
        printf("%d: ", *c);
        for (i=0; i<k/4; i++)
            printf("%d ", sol[i]);
        printf("\t%d\t%d", localMax, *globalMax);
        printf("\n");

        return cnt+1;
    }
    for (i=start; i<n; i++) {
        sol[pos] = val[i];
        cnt = comb(pos+1, val, sol, n, k, i+1, cnt, globalMax, globalSol, globalSolLen, c, st);
    }
    return cnt;
}

bool checkVal(int *muri, int n){

    vector<Muro> strMuri(n);

    for(int i = 0; i < n; i++){
        strMuri[i] = {muri[i] % 2, (muri[i] / 2) % (LATO-1), (muri[i] / 2) / (LATO-1)};
    }

    for(int i = 0; i < n-1; i++){
        if(overlap(strMuri[i], strMuri[n-1])){
            return false;
        }
    }

    return true;
}

bool overlap(Muro a, Muro b){

    if(a.dir != b.dir && a.xc == b.xc && a.yc == b.yc) return true;
    if(a.dir == b.dir){
        if(a.dir == HORIZ && a.xc == b.xc && abs(a.yc - b.yc) <= 1) return true;  // v
        if(a.dir == VERT && a.yc == b.yc && abs(a.xc - b.xc) <= 1) return true;  // h
    }

    return false;
}

bool checkRagg(int *muri, int n, vector<vector<bool>> ragg, int *maxQueueDim, Player *sol){

    int max1, max2, px, py;
    Player d1, d2;
    bool r = true, r1, r2;

    vector<Muro> strMuri(n);
    vector<vector<Player>> parent1(LATO, vector<Player>(LATO));
    vector<vector<Player>> parent2(LATO, vector<Player>(LATO));

    for(int i = 0; i < n; i++){
        strMuri[i] = {muri[i] % 2, (muri[i] / 2) % (LATO-1), (muri[i] / 2) / (LATO-1)};
    }

    for(int i = 0; i < LATO; i++){
        for(int j = 0; j < LATO; j++){
            parent1[i][j] = parent2[i][j] = {-1, -1};
        }
    }

    max1 = max2 = 0;
    d1 = d2 = {-1, -1};
    px = py = -1;
    for(int x = 0; x < LATO; x++){
        for(int y = 0; y < LATO; y++){
            //if(ragg[x][y]){
                Player p = {x, y};
                r1 = r2 = true;
                if(y > 0){
                    r1 = checkReachability(p, strMuri, 0, parent1, &max1, &d1);
                }
                if(y < 6){
                    r2 = checkReachability(p, strMuri, 6, parent2, &max2, &d2);
                }

                ragg[x][y] = r1 && r2;
                r = r && ragg[x][y];
                *maxQueueDim = max(*maxQueueDim, max(max1, max2));
                if(*maxQueueDim == max(max1, max2)){
                    *sol = {x, y};
                }
            //}
            px = x;
            py = y;
        }
    }

    return r;
}

// BFS
bool checkReachability(Player p, vector<Muro> muri, int yEnd, vector<vector<Player>> paret, int *maxQueueDim, Player *dest){

    const int moves[4][2] = {
            {-1, 0},		// LEFT
            {0, -1},		// UP
            {0, 1},			// DOWN
            {1, 0}			// RIGHT
    };

    int maxcnt;
    Player currElem, adjElem;

    vector<vector<bool>> enqueued(LATO, vector<bool>(LATO));

    queue<Player> q;
    q.push(p);
    enqueued[p.xp][p.yp] = true;

    maxcnt = 1;
    while(!q.empty()){
        currElem = q.front();
        q.pop();

        if(currElem.yp == yEnd){
            *maxQueueDim = maxcnt;
            *dest = currElem;
            return true;
        }

        for(int i = 0; i < 4; i++){
            adjElem.xp = currElem.xp + moves[i][0];
            adjElem.yp = currElem.yp + moves[i][1];

            if(validPos(adjElem, currElem, muri)){
                if(!enqueued[adjElem.xp][adjElem.yp]) {
                    q.push(adjElem);
                    enqueued[adjElem.xp][adjElem.yp] = true;
                }
            }
        }
        maxcnt = max(maxcnt, (int)q.size());
    }

    *maxQueueDim = maxcnt;
    return false;
}

bool wallBetween(Player dest, Player src, Muro m){
    if(m.dir == HORIZ){
        if(src.xp == dest.xp && (m.xc == dest.xp || dest.xp == m.xc + 1) &&
                (src.yp <= m.yc && m.yc < dest.yp || dest.yp <= m.yc && m.yc < src.yp)) return true;
    }
    else{
        if(src.yp == dest.yp && (m.yc == dest.yp || dest.yp == m.yc + 1) &&
                (src.xp <= m.xc && m.xc < dest.xp || dest.xp <= m.xc && m.xc < src.xp)) return true;
    }

    return false;
}

bool validPos(Player dest, Player src, vector<Muro> muri){
    if(!(0 <= dest.xp && dest.xp < LATO && 0 <= dest.yp && dest.yp < LATO)) return false;

    for(Muro m : muri){
        if(wallBetween(dest, src, m))
            return false;
    }

    return true;
}
