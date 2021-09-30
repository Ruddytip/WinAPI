#include "model.hpp"

void Model::line(HDC hdc, Vec2i t0, Vec2i t1, const COLORREF &color) {
    int x0 = t0.x, y0 = t0.y;
    int x1 = t1.x, y1 = t1.y;
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        SetPixel(hdc, (steep?y:x), (steep?x:y), color);
        error2 += derror2;

        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

double range(double data, double old_min, double old_max, double new_min, double new_max){
    double old_range = old_max - old_min;
    double new_range = new_max - new_min;
    return (((data - old_min) * new_range) / old_range) + new_min;
}

double helpPointPos(Vec2i p, Vec3d a, Vec3d b){
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

bool inTriangle(Vec2i p,  Vec3d* t){
    double q0 = helpPointPos(p, t[0], t[1]);
    double q1 = helpPointPos(p, t[1], t[2]);
    double q2 = helpPointPos(p, t[2], t[0]);
    return (q0 >= 0 && q1 >= 0 && q2 >= 0) || (q0 <= 0 && q1 <= 0 && q2 <= 0);
}

bool Model::helpZ(Vec2i point, Vec3d* t){
    if(point.x<0 || point.y<0 || point.x>=size_screen.x || point.y>=size_screen.y) return false;
    Vec3d normal = (t[0] - t[1]) ^ (t[0] - t[2]);
    double z = (-normal.x * (point.x - t[0].x) - normal.y * (point.y - t[0].y) + normal.z * t[0].z) / normal.z;
    unsigned long index = point.x + point.y * size_screen.x;
        if(z > z_buffer[index]){
            z_buffer[index] = z;
            return true;
        }
    return false;
}

void Model::triangle(HDC hdc, Vec3d* t, const COLORREF &color){
    Vec3d Min = t[0], Max = t[0];
    for(int i = 1; i < 3; ++i){
        Min.x = t[i].x < Min.x? t[i].x: Min.x; Max.x = t[i].x > Max.x? t[i].x: Max.x;
        Min.y = t[i].y < Min.y? t[i].y: Min.y; Max.y = t[i].y > Max.y? t[i].y: Max.y;
    }

    for(int j = Min.y; j <= Max.y; ++j){
        for(int i = Min.x; i <= Max.x; ++i){
            Vec2i point(i, j);
            if(inTriangle(point, t))
                if(helpZ(point, t))
                    SetPixel(hdc, i, j, color);
        }
    }
}

void Model::triangle(HDC hdc, Vec3d* t, int count, int n, double nor){
    Vec3d Min = t[0], Max = t[0];
    for(int i = 1; i < 3; ++i){
        Min.x = t[i].x < Min.x? t[i].x: Min.x; Max.x = t[i].x > Max.x? t[i].x: Max.x;
        Min.y = t[i].y < Min.y? t[i].y: Min.y; Max.y = t[i].y > Max.y? t[i].y: Max.y;
    }

    Vec2d Mint = Vec2d(uv[groups[count].faces[n].uv_cords[0]].x, uv[groups[count].faces[n].uv_cords[0]].y);
    Vec2d Maxt = Vec2d(uv[groups[count].faces[n].uv_cords[0]].x, uv[groups[count].faces[n].uv_cords[0]].y);
    for(int i = 1; i < 3; ++i){
        Mint.x = uv[groups[count].faces[n].uv_cords[i]].x<Mint.x?uv[groups[count].faces[n].uv_cords[i]].x:Mint.x;
        Mint.y = uv[groups[count].faces[n].uv_cords[i]].y<Mint.y?uv[groups[count].faces[n].uv_cords[i]].y:Mint.y;
        Maxt.x = uv[groups[count].faces[n].uv_cords[i]].x>Maxt.x?uv[groups[count].faces[n].uv_cords[i]].x:Maxt.x;
        Maxt.y = uv[groups[count].faces[n].uv_cords[i]].y>Maxt.y?uv[groups[count].faces[n].uv_cords[i]].y:Maxt.y;
    }

    for(int j = Min.y; j <= Max.y; ++j){
        for(int i = Min.x; i <= Max.x; ++i){
            Vec2i point(i, j);
            if(inTriangle(point, t))
                if(helpZ(point, t)){
                    // double x = range(i, Max.x, Min.x, Mint.x, Maxt.x);
                    // double y = range(j, Min.y, Max.y, Mint.y, Maxt.y);
                    int c = range(groups[count].faces[n].material_id, 1, materials.size(), 0, 255);
                    // TGAColor c = texture.get(x * texture.get_width(), y * texture.get_height());
                    SetPixel(hdc, i, j, RGB(c, c, c));
                }                        
        }
    }
}

Model::Model(std::string filename, Vec2i _size_screen) : groups(), textures(), materials(), size_screen(_size_screen) {
    std::ifstream in;
    in.open(filename + "/model.obj", std::ifstream::in);
    if (in.fail()) return;
    std::string line, trash;
    int index = 0;
    while (!in.eof()) {
        std::getline(in, line);
        std::stringstream iss(line);
        if(!line.compare(0, 7, "mtllib ")) {
            initMaterials(filename + "/" + line.erase(0, 7));
            initTextures();
            initMaps();
        }
        if(!line.compare(0, 7, "usemtl ")) {
            trash = line.erase(0, 7);
            for(unsigned int i = 0; i < materials.size(); ++i) if(materials[i].name == trash) {index = i; break;}
        }
        if(!line.compare(0, 2, "g ")){
            group g;
            g.name = line.erase(0, 2);
            g.visible = true;
            groups.push_back(g);
        }
        if(!line.compare(0, 2, "o ")){
            object obj;
            obj.name = line.erase(0, 2);
            obj.visible = true;
            groups.end()->objects.push_back(obj);
        }
        if(!line.compare(0, 2, "v ")) {
            iss >> trash;
            std::string data[3];
            for(int i = 0; i < 3; ++i) iss >> data[i];
            Vec3d v(std::stod(data[0]), std::stod(data[1]), std::stod(data[2]));
            groups.end()->objects.end()->verts.push_back(v);
        }
        if(!line.compare(0, 3, "vt ")){
            iss >> trash;
            std::string data[2];
            for(int i = 0; i < 2; ++i) iss >> data[i];
            Vec2d v(std::stod(data[0]), std::stod(data[1]));
            groups.end()->objects.end()->uv.push_back(v);
        }
        if(!line.compare(0, 3, "vn ")){
            iss >> trash;
            std::string data[3];
            for(int i = 0; i < 3; ++i) iss >> data[i];
            Vec3d v(std::stod(data[0]), std::stod(data[1]), std::stod(data[2]));
            groups.end()->objects.end()->normals.push_back(v);
        }
        if(!line.compare(0, 2, "f ")) {
            iss >> trash;
            face f;
            std::string data[4];

            for(int i = 0; i < 3; ++i){
                iss >> data[i];
                int count = 0;
                for(auto j = data[i].begin(); j <= data[i].end(); ++j) if(*j == '/') ++count;
                if(count == 0){
                    f.id_v.push_back(std::stoi(data[i]) - 1);
                }else
                if(count == 1){
                    int pos = data[i].find('/');
                    f.id_v.push_back(std::stoi(data[i].substr(0, pos)) - 1);
                    f.id_vt.push_back(std::stoi(data[i].substr(pos + 1, data[i].length() - pos + 1)) - 1);
                }else
                if(count == 2){
                    int pos1 = data[i].find('/');
                    int pos2 = data[i].rfind('/');
                    f.id_v.push_back(std::stoi(data[i].substr(0, pos1)) - 1);
                    if((pos2 - pos1) > 1){
                        f.id_vt.push_back(std::stoi(data[i].substr(pos1 + 1, pos2 - pos1 - 1)) - 1);
                    }
                    f.id_vn.push_back(std::stoi(data[i].substr(pos2 + 1, data[i].length() - pos2 + 1)) - 1);
                }
            }
            // f.material_id = index;
            groups.end()->objects.end()->faces.push_back(f);

            iss >> data[3];

            if(data[3].length() > 0){
                face f2;
                int count = 0;
                for(auto j = data[3].begin(); j<= data[3].end(); j++) if(*j == '/') ++count;        

                if(count == 0){
                    f2.id_v.push_back(f.id_v[0]);
                    f2.id_v.push_back(f.id_v[2]);
                    f2.id_v.push_back(std::stoi(data[3]) - 1);
                }else
                if(count == 1){
                    int pos = data[3].find('/');
                    f2.id_v.push_back(f.id_v[0]);
                    f2.id_v.push_back(f.id_v[2]);
                    f2.id_v.push_back(std::stoi(data[3].substr(0, pos)) - 1);

                    f2.id_vt.push_back(f.id_vt[0]);
                    f2.id_vt.push_back(f.id_vt[2]);
                    f2.id_vt.push_back(std::stoi(data[3].substr(pos + 1, data[3].length() - pos + 1)) - 1);

                }else
                if(count == 2){
                    int pos1 = data[3].find('/');
                    int pos2 = data[3].rfind('/');
                    f2.id_v.push_back(f.id_v[0]);
                    f2.id_v.push_back(f.id_v[2]);                    
                    f2.id_v.push_back(std::stoi(data[3].substr(0, pos1)) - 1);
                    if((pos2 - pos1) > 1){
                        f2.id_vt.push_back(f.id_vt[0]);
                        f2.id_vt.push_back(f.id_vt[2]);
                        f2.id_vt.push_back(std::stoi(data[3].substr(pos1 + 1, pos2 - pos1 - 1)) - 1);
                    }
                    f2.id_vn.push_back(f.id_vn[0]);
                    f2.id_vn.push_back(f.id_vn[2]);
                    f2.id_vn.push_back(std::stoi(data[3].substr(pos2 + 1, data[3].length() - pos2 + 1)) - 1);
                }
                // f2.material_id = index;
                groups.end()->objects.end()->faces.push_back(f2);                
            }
        }
    }

    nameModel = filename.substr(filename.rfind('/') + 1, filename.length() - filename.rfind('/') - 1);
    z_buffer = new double[size_screen.x * size_screen.y];
    for(int i = 0; i < size_screen.x * size_screen.y; ++i) z_buffer[i] = -std::numeric_limits<double>::max();

    min = max = groups[0].objects[0].verts[0];
    for(int i = 0; i < groups.size(); ++i)
        for(int j = 0; j < groups[i].objects.size(); ++j)
                for(int c = 0; c < 3; ++c){
                    min.x = groups[i].objects[j].verts[c].x < min.x ? groups[i].objects[j].verts[c].x : min.x;
                    min.y = groups[i].objects[j].verts[c].y < min.y ? groups[i].objects[j].verts[c].y : min.y;
                    max.x = groups[i].objects[j].verts[c].x > max.x ? groups[i].objects[j].verts[c].x : max.x;
                    max.y = groups[i].objects[j].verts[c].y > max.y ? groups[i].objects[j].verts[c].y : max.y;
                }

    size = Vec3d(max.x - min.x, max.y - min.y, max.z - min.z);
    scale = (size_screen.x / size.x)<(size_screen.y / size.y) ? (size_screen.x / size.x) : (size_screen.y / size.y);

    for(unsigned int i = 0; i < groups.size(); ++i) groups[i].visible = true;
    initGroups();
    // printInfo();
}

void Model::initMaterials(std::string filename){
    material m = {"Default", Vec3d(1, 1, 1), Vec3d(1, 1, 1), Vec3d(0, 0, 0), Vec3d(0, 0, 0), 0.0, 0.0, 1.0, 0, "", "", "", ""};
    materials.push_back(m);
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line, trash;
    while (!in.eof()) {
        std::getline(in, line);
        if(!line.compare(0, 7, "newmtl ")){
            m = {line.erase(0, 7), Vec3d(1, 1, 1), Vec3d(1, 1, 1), Vec3d(0, 0, 0), Vec3d(0, 0, 0), 0.0, 0.0, 1.0, 0, "", "", "", ""};
            while(!in.eof() && line.length()>0){
                std::getline(in, line);
                std::stringstream iss(line);
                if(!line.compare(0, 3, "Ka ")){
                    iss >> trash;
                    iss >> trash; m.Ka.x = std::stod(trash);
                    iss >> trash; m.Ka.y = std::stod(trash);
                    iss >> trash; m.Ka.z = std::stod(trash);
                }
                if(!line.compare(0, 3, "Kd ")){
                    iss >> trash;
                    iss >> trash; m.Kd.x = std::stod(trash);
                    iss >> trash; m.Kd.y = std::stod(trash);
                    iss >> trash; m.Kd.z = std::stod(trash);
                }
                if(!line.compare(0, 3, "Ke ")){
                    iss >> trash;
                    iss >> trash; m.Ke.x = std::stod(trash);
                    iss >> trash; m.Ke.y = std::stod(trash);
                    iss >> trash; m.Ke.z = std::stod(trash);
                }
                if(!line.compare(0, 3, "Ks ")){
                    iss >> trash;
                    iss >> trash; m.Ks.x = std::stod(trash);
                    iss >> trash; m.Ks.y = std::stod(trash);
                    iss >> trash; m.Ks.z = std::stod(trash);
                }
                if(!line.compare(0, 3, "Ns ")){
                    iss >> trash;
                    iss >> trash; m.Ns = std::stod(trash);
                }
                if(!line.compare(0, 3, "Ni ")){
                    iss >> trash;
                    iss >> trash; m.Ni = std::stod(trash);
                }
                if(!line.compare(0, 2, "d ")){
                    iss >> trash;
                    iss >> trash; m.d = std::stod(trash);
                }
                if(!line.compare(0, 7, "map_Ka ")){
                    iss >> trash;
                    iss >> trash; m.map_Ka = trash;
                }
                if(!line.compare(0, 7, "map_Kd ")){
                    iss >> trash;
                    iss >> trash; m.map_Kd = trash;
                }
                if(!line.compare(0, 7, "map_Ks ")){
                    iss >> trash;
                    iss >> trash; m.map_Ks = trash;
                }
                if(!line.compare(0, 6, "map_D ")){
                    iss >> trash;
                    iss >> trash; m.map_D = trash;
                }
            }
            materials.push_back(m);
        }
    }
}

void Model::initTextures(){
    
}

void Model::initMaps(){
    
}

void Model::initGroups(){
    if(nameModel == "Orc"){
        // groups[0].visible = false;  //Левый глаз
        // groups[1].visible = false;  //Правый глаз
        // groups[2].visible = false;  //Тело
        // groups[3].visible = false;  //Нижние дёсна
        // groups[4].visible = false;  //Зуб верхний
        // groups[5].visible = false;  //Зуб верхний
        // groups[6].visible = false;  //Зуб верхний
        // groups[7].visible = false;  //Зуб верхний
        // groups[8].visible = false;  //Зуб верхний
        // groups[9].visible = false;  //Зуб верхний
        // groups[10].visible = false; //Зуб нижний
        // groups[11].visible = false; //Зуб нижний
        // groups[12].visible = false; //Зуб нижний
        // groups[13].visible = false; //Зуб нижний
        // groups[14].visible = false; //Клык малый правый
        // groups[15].visible = false; //Клык малый левый
        // groups[16].visible = false; //Клык большой правый
        // groups[17].visible = false; //Клык большой левый
        // groups[18].visible = false; //Кольцо в правом ухе заднее
        // groups[19].visible = false; //Кольцо в правом ухе переднее
        // groups[20].visible = false; //Кольцо в левом ухе заднее
        // groups[21].visible = false; //Кольцо в левом ухе переднее
        // groups[22].visible = false; //Кольцо на клыке
        // groups[23].visible = false; //Кольцо в носу
        groups[24].visible = false; //?
        groups[25].visible = false; //?
    }
    if(nameModel == "Elizabeth"){
        // groups[0].visible = false;  //Тело
        // groups[1].visible = false;  //Напальчник
        groups[2].visible = false;  //Ресницы
        // groups[3].visible = false;  //Волосы
        // groups[4].visible = false;  //Брош
        // groups[5].visible = false;  //Сорочка
        // groups[6].visible = false;  //Юбка
        groups[7].visible = false;  //Тело
        groups[8].visible = false;  //Тело
        groups[9].visible = false;  //Подтяжки
        // groups[10].visible = false; //Чулки
        // groups[11].visible = false; //Туфли
        groups[12].visible = false; //Цилиндр?
    }
}

void Model::printInfo(){
    unsigned long count = 0;
    std::cerr << nameModel << "\nGroups count - " << groups.size();
    for(unsigned int i = 0; i < groups.size(); ++i){
        std::cerr << std::endl << i << " - " << groups[i].name << std::endl;
        std::cerr << "faces - " << groups[i].faces.size() << std::endl;
        count += groups[i].faces.size();
    }
    std::cerr << "\nTotal verts - " << verts.size() << "\nTotal texture cords - " << uv.size() << "\nTotal normals - "  << normals.size() << "\nTotal faces - "  << count << std::endl;
}

Model::~Model() {
    delete[] z_buffer;
}

void Model::draw(HDC hdc){
    drawMeshTriangle(hdc);
    // drawMeshTexture(hdc);
    // drawMesh(hdc, RGB(255, 255, 255));
    // drawZ_buffer(hdc);
}

void Model::drawZ_buffer(HDC hdc){
    for(int i = 0; i < size_screen.x * size_screen.y; ++i) z_buffer[i] = -std::numeric_limits<double>::max();
    for(unsigned int count = 0; count < groups.size(); ++count){
        if(!groups[count].visible) continue;
        for (unsigned int i = 0; i < groups[count].faces.size(); ++i) {
            Vec3d screen_coords[3];
            Vec3d world_coords[3];
            for (int j = 0; j < 3; j++) {
                world_coords[j] = verts[groups[count].faces[i].vert_cords[j]];
                screen_coords[j] = (world_coords[j] - min) * scale;
                screen_coords[j].y = size_screen.y - screen_coords[j].y;
            }
            Vec3d normal = (world_coords[0] - world_coords[1]) ^ (world_coords[0] - world_coords[2]);
            double intensity = normal.getCosAngle(Vec3d(0.0, 0.0, 1.0));
            if (intensity > 0) {
                Vec3d Min = screen_coords[0], Max = screen_coords[0];
                for(int k = 1; k < 3; ++k){
                    Min.x = screen_coords[k].x < Min.x? screen_coords[k].x: Min.x;
                    Max.x = screen_coords[k].x > Max.x? screen_coords[k].x: Max.x;
                    Min.y = screen_coords[k].y < Min.y? screen_coords[k].y: Min.y;
                    Max.y = screen_coords[k].y > Max.y? screen_coords[k].y: Max.y;
                }
                for(int j = Min.y; j <= Max.y; ++j)
                    for(int i = Min.x; i <= Max.x; ++i)
                        if(inTriangle(Vec2i(i, j), screen_coords)) helpZ(Vec2i(i, j), screen_coords);
            }
        }
    }
    for(int j = 0; j < size_screen.y; ++j)
        for(int i = 0; i < size_screen.x; ++i){
            int converted = range(z_buffer[i + j * size_screen.x], (min.z - min.z) * scale, (max.z - min.z) * scale, 0, 255);
            SetPixel(hdc, i, j, RGB(converted, converted, converted));
        }
}

void Model::drawMesh(HDC hdc, const COLORREF &color){
    for(unsigned int count = 0; count < groups.size(); ++count){
        if(!groups[count].visible) continue;
        for (unsigned int i = 0; i < groups[count].faces.size(); ++i) {
            Vec2i screen_coords[3];
            for (int j = 0; j < 3; ++j) {
                Vec3d world_coords = verts[groups[count].faces[i].vert_cords[j]];
                world_coords = (world_coords - min) * scale;
                screen_coords[j] = Vec2i(world_coords.x, size_screen.y - world_coords.y);
            }
            line(hdc, screen_coords[0], screen_coords[1], color);
            line(hdc, screen_coords[1], screen_coords[2], color);
            line(hdc, screen_coords[2], screen_coords[0], color);
        }
    }
}

void Model::drawMeshTriangle(HDC hdc){
    for(int i = 0; i < size_screen.x * size_screen.y; ++i) z_buffer[i] = -std::numeric_limits<double>::max();
    for(unsigned int count = 0; count < groups.size(); ++count){
        if(!groups[count].visible) continue;
        for (unsigned int i = 0; i < groups[count].faces.size(); ++i) {
            Vec3d screen_coords[3];
            Vec3d world_coords[3];
            for (int j = 0; j < 3; j++) {
                world_coords[j] = verts[groups[count].faces[i].vert_cords[j]];
                screen_coords[j] = (world_coords[j] - min) * scale;
                screen_coords[j].y = size_screen.y - screen_coords[j].y;
            }
            Vec3d normal = (world_coords[0] - world_coords[1]) ^ (world_coords[0] - world_coords[2]);
            double intensity = normal.getCosAngle(Vec3d(0.0, 0.0, 1.0));
            if (intensity > 0) {
                int color = intensity * 255;
                triangle(hdc, screen_coords, RGB( color, color, color));
            }
        }
    }
}

void Model::drawMeshTexture(HDC hdc){
    for(int i = 0; i < size_screen.x * size_screen.y; ++i) z_buffer[i] = -std::numeric_limits<double>::max();
    for(unsigned int count = 0; count < groups.size(); ++count){
        if(!groups[count].visible) continue;
        for (unsigned int i = 0; i < groups[count].faces.size(); ++i) {
            Vec3d screen_coords[3];
            Vec3d world_coords[3];
            for (int j = 0; j < 3; j++) {
                world_coords[j] = verts[groups[count].faces[i].vert_cords[j]];
                screen_coords[j] = (world_coords[j] - min) * scale;
                screen_coords[j].y = size_screen.y - screen_coords[j].y;
            }
            Vec3d normal = (world_coords[0] - world_coords[1]) ^ (world_coords[0] - world_coords[2]);
            double intensity = normal.getCosAngle(Vec3d(0.0, 0.0, 1.0));
            if(intensity > 0)
                triangle(hdc, screen_coords, count, i, intensity);
        }
    }
}
