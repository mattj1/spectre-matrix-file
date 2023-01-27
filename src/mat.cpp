//
//  mat.cpp
//  matloader
//
//  Created by Matthew Johnson on 1/25/23.
//

#include "mat.h"
#include <stdio.h>


static int read_short(FILE *f) {
    unsigned short val;
    fread(&val, sizeof(unsigned short), 1, f);

    val = (val >> 8) | (val << 8);
    return val;
}

static MatrixFile::MatModelVert read_vertex(FILE *f) {
   
    MatrixFile::MatModelVert vert;
    vert.x = read_short(f);
    read_short(f);
    vert.y = read_short(f);
    read_short(f);
    vert.z = read_short(f);
    read_short(f);
    
    return vert;
}

struct entry_t {
    char entry_type[5];
    short entry_id;
    unsigned short offset;
    unsigned short length;
};

namespace MatrixFile {

    MatFile *LoadFromFile(FILE *f) {
        std::vector <entry_t> entries;

        MatrixFile::MatFile *mat = new MatrixFile::MatFile();

        fseek(f, 4, SEEK_SET);

        int num_entries = read_short(f);
//        printf("Num entries: %d\n", num_entries);

        for (int i = 0; i < num_entries; i++) {
            entry_t entry;
            memset(&entry, 0, sizeof(entry_t));
            fread(&entry.entry_type, 4, 1, f);
            entry.entry_id = read_short(f);

            fseek(f, 2, SEEK_CUR);
            entry.offset = read_short(f);
            fseek(f, 2, SEEK_CUR);
            entry.length = read_short(f);

//            printf("Entry %s %d %d %d\n", entry.entry_type, entry.entry_id, entry.offset, entry.length);

            entries.push_back(entry);
        }

        for (auto it = entries.begin(); it != entries.end(); ++it) {
            auto entry = (*it);
            fseek(f, entry.offset, SEEK_SET);

            if (!strcmp(it->entry_type, "wrld")) {
                MatrixFile::MatMap matMap;
                matMap.id = entry.entry_id;
                int num_obj = read_short(f);

                for (int j = 0; j < num_obj; j++) {
                    MatrixFile::MatMapObject matMapObj;
                    matMapObj.obj_type = read_short(f);
                    matMapObj.x = 1024 - read_short(f);
                    matMapObj.y = read_short(f);
                    matMapObj.angle = read_short(f);

                    matMap.objects.push_back(matMapObj);
                }

                mat->maps.push_back(matMap);
            }

            if (!strcmp(it->entry_type, "enem")) {
                MatrixFile::MatEnemySet enemySet;
                enemySet.id = entry.entry_id;
                int num_sets = read_short(f);

                for (int j = 0; j < num_sets; j++) {
                    MatrixFile::MatEnemySetEntry e;
                    e.enemyType = read_short(f);
                    e.minAtStart = read_short(f);
                    e.maxAtStart = read_short(f);
                    e.regen = read_short(f);

                    enemySet.entries.push_back(e);
                }

                // printf("Enemy set %d, num entries: %d\n", enemySet.id, enemySet.entries.size());
                mat->enemySets.push_back(enemySet);
            }

            if (!strcmp(it->entry_type, "wave")) {
                MatrixFile::MatDifficulty d;
                int num_levels = read_short(f);
                //            printf("Difficulty, num levels: %d\n", num_levels);
                for (int i = 0; i < num_levels; i++) {
                    MatrixFile::MatLevel level;
                    level.mapId = read_short(f);
                    level.enemySetId = read_short(f);
                    level.weaponActivate = read_short(f);
                    level.enemyDataSetId = read_short(f);
                    level.caption = read_short(f);
                    read_short(f); // 03 FD
                    level.songId = read_short(f);
                    read_short(f); // 03 E7

                    //                printf("Level with map %d\n", level.mapId);
                    d.levels.push_back(level);
                }

                mat->difficulties.push_back(d);
            }
            
            if(!strcmp(it->entry_type, "shap")) {
                MatrixFile::MatModel model;
                model.id = entry.entry_id;
                
                int num_verts = read_short(f) + 1;
                
                for(int i = 0; i < num_verts; i++) {
                    MatrixFile::MatModelVert vert = read_vertex(f);
                    model.verts.push_back(vert);
                }
                
                int num_faces = read_short(f) + 1;
                
                for(int i = 0; i < num_faces; i++) {
                    MatrixFile::MatModelFace face;
                    
                    int num_elements = read_short(f) + 1;
                    
                    for(int i = 0; i < num_elements; i++) {
                        unsigned short vertIndex = read_short(f) - 1;
                        face.vertIndices.push_back(vertIndex);
                    }
                    
                    face.flags = read_short(f);
                    
                    model.faces.push_back(face);
                }
                
                // Read footprint
                
                num_verts = read_short(f) + 1;
                
                for(int i = 0; i < num_verts; i++) {
                    MatrixFile::MatModelVert vert = read_vertex(f);
                    model.footprintVerts.push_back(vert);
                }
                
                int num_footprint_edges = read_short(f) + 1;
                
                for(int i = 0; i < num_footprint_edges; i++) {
                    MatrixFile::MatModelFootprintEdge edge;
                    edge.verts[0] = read_short(f) - 1;
                    edge.verts[1] = read_short(f) - 1;
                    model.footprintEdges.push_back(edge);
                }
                
                mat->models.push_back(model);
            }
            
            if(!strcmp(it->entry_type, "rshp")) {
                MatModelVariations variations;
                
                int numVariations = read_short(f);
                
                for(int i = 0; i < numVariations; i++) {
                    variations.modelIDs.push_back(read_short(f));
                }
                
                mat->modelVariations.push_back(variations);
            }
            
            if(!strcmp(it->entry_type, "mshp")) {
                MatCompoundModel model;
                model.id = entry.entry_id;
                
                int numModels = read_short(f);
                
                for(int i = 0; i < numModels; i++) {
                    MatrixFile::MatCompoundModelElement element;
                    element.modelId = read_short(f);
                    
                    for(int j = 0; j < 3; j++) {
                        element.offset[j] = read_short(f);
                    }
                    
                    model.elements.push_back(element);
                }
                
                
                mat->compoundModels.push_back(model);
            }
            
        }
        return mat;
    }
}
