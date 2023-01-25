//
//  main.cpp
//  matloader
//
//  Created by Matthew Johnson on 1/25/23.
//

#include "mat.h"

int main(int argc, const char * argv[]) {
    
    FILE *f = fopen("CENTRAL.MAT", "rb");
    if(!f) {
        printf("File not found\n");
        return 0;
    }
    
    MatrixFile::LoadFromFile(f);

    return 0;
}
