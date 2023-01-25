//
//  mat.h
//  matloader
//
//  Created by Matthew Johnson on 1/25/23.
//

#ifndef mat_h
#define mat_h

#include <iostream>
#include <vector>
#include <stdio.h>

namespace MatrixFile {

    class MatMapObject {
    public:
        short x, y, obj_type, angle;
    };

    class MatEnemySetEntry {
    public:
        unsigned short enemyType;
        unsigned short minAtStart;
        unsigned short maxAtStart;
        unsigned short regen;
    };

    class MatEnemySet {
    public:
        int id;
        std::vector <MatEnemySetEntry> entries;
    };

    class MatMap {
    public:
        int id;
        std::vector <MatMapObject> objects;
    };

    class MatLevel {
    public:
        unsigned short mapId;
        unsigned short enemySetId;
        unsigned short weaponActivate;
        unsigned short enemyDataSetId;
        unsigned short caption;
        unsigned short songId;
    };

    class MatDifficulty {

    public:
        std::vector <MatLevel> levels;
    };


    class MatFile {

    public:

        std::vector <MatMap> maps;
        std::vector <MatEnemySet> enemySets;
        std::vector <MatDifficulty> difficulties;
    };

    MatFile *LoadFromFile(FILE *f);

}

#endif /* mat_h */
