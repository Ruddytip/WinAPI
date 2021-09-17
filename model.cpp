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

void Model::triangle(HDC hdc, Vec3d* t, int n, double nor){
    Vec3d Min = t[0], Max = t[0];
    for(int i = 1; i < 3; ++i){
        Min.x = t[i].x < Min.x? t[i].x: Min.x; Max.x = t[i].x > Max.x? t[i].x: Max.x;
        Min.y = t[i].y < Min.y? t[i].y: Min.y; Max.y = t[i].y > Max.y? t[i].y: Max.y;
    }

    Vec2d Mint = Vec2d(uv[faces[n].uv[0]].x, uv[faces[n].uv[0]].y);
    Vec2d Maxt = Vec2d(uv[faces[n].uv[0]].x, uv[faces[n].uv[0]].y);
    for(int i = 1; i < 3; ++i){
        Mint.x = uv[faces[n].uv[i]].x<Mint.x?uv[faces[n].uv[i]].x:Mint.x;
        Mint.y = uv[faces[n].uv[i]].y<Mint.y?uv[faces[n].uv[i]].y:Mint.y;
        Maxt.x = uv[faces[n].uv[i]].x>Maxt.x?uv[faces[n].uv[i]].x:Maxt.x;
        Maxt.y = uv[faces[n].uv[i]].y>Maxt.y?uv[faces[n].uv[i]].y:Maxt.y;
    }

    for(int j = Min.y; j <= Max.y; ++j){
        for(int i = Min.x; i <= Max.x; ++i){
            Vec2i point(i, j);
                if(inTriangle(point, t))
                    if(helpZ(point, t)){
                        double x = range(i, Min.x, Max.x, Mint.x, Maxt.x);
                        double y = range(j, Max.y, Min.y, Mint.y, Maxt.y);
                        TGAColor c = texture.get(x * texture.get_width(), y * texture.get_height());
                        SetPixel(hdc, i, j, RGB(c.r * nor, c.g * nor, c.b * nor));
                    }                        
        }
    }
}

Model::Model(std::string filename, Vec2i _size_screen) : verts(), uv(), normals(), faces(), groups(), size_screen(_size_screen) {
    std::ifstream in;
    int o = 0;
    in.open(filename + "/source/model.obj", std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    std::string trash;
    while (!in.eof()) {
        std::getline(in, line);
        std::stringstream iss(line);
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3d v;
            std::string data[3];
            for(int i = 0; i < 3; ++i) iss >> data[i];
            v.x = std::stod(data[0]);
            v.y = std::stod(data[1]);
            v.z = std::stod(data[2]);
            if(!verts.size()){
                min = max = v;
            }else{
                min.x = v.x < min.x ? v.x: min.x;
                min.y = v.y < min.y ? v.y: min.y;
                min.z = v.z < min.z ? v.z: min.z;
                max.x = v.x > max.x ? v.x: max.x;
                max.y = v.y > max.y ? v.y: max.y;
                max.z = v.z > max.z ? v.z: max.z;
            }
            verts.push_back(v);
        }
        if(!line.compare(0, 2, "vt")){
            iss >> trash;
            Vec2d v;
            std::string data[2];
            for(int i = 0; i < 2; ++i) iss >> data[i];
            v.x = std::stod(data[0]);
            v.y = std::stod(data[1]);
            uv.push_back(v);
        }
        if(!line.compare(0, 2, "vn")){
            iss >> trash;
            Vec3d v;
            std::string data[3];
            for(int i = 0; i < 3; ++i) iss >> data[i];
            v.x = std::stod(data[0]);
            v.y = std::stod(data[1]);
            v.z = std::stod(data[2]);
            normals.push_back(v);
        }
        if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            face f;
            std::string data[4];

            for(int i = 0; i < 3; ++i){
                iss >> data[i];
                int count = 0;
                for(auto j = data[i].begin(); j <= data[i].end(); ++j) if(*j == '/') ++count;
                if(count == 0){
                    f.verts.push_back(  std::stoi(data[i]) - 1);
                }else
                if(count == 1){
                    int pos = data[i].find('/');
                    f.verts.push_back(  std::stoi(data[i].substr(0, pos)) - 1);
                    f.uv.push_back(     std::stoi(data[i].substr(pos + 1, data[i].length() - pos + 1)) - 1);
                }else
                if(count == 2){
                    int pos1 = data[i].find('/');
                    int pos2 = data[i].rfind('/');
                    f.verts.push_back(  std::stoi(data[i].substr(0, pos1)) - 1);
                    if((pos2 - pos1) > 1){
                        f.uv.push_back( std::stoi(data[i].substr(pos1 + 1, pos2 - pos1 - 1)) - 1);
                    }
                    f.normals.push_back(std::stoi(data[i].substr(pos2 + 1, data[i].length() - pos2 + 1)) - 1);
                }
            }
            faces.push_back(f);

            iss >> data[3];

            if(data[3].length() > 0){
                face f2;
                int count = 0;
                for(auto j = data[3].begin(); j<= data[3].end(); j++) if(*j == '/') ++count;        

                if(count == 0){
                    f2.verts.push_back(f.verts[0]);
                    f2.verts.push_back(f.verts[2]);
                    f2.verts.push_back(std::stoi(data[3]) - 1);
                }else
                if(count == 1){
                    int pos = data[3].find('/');
                    f2.verts.push_back(f.verts[0]);
                    f2.verts.push_back(f.verts[2]);
                    f2.verts.push_back(std::stoi(data[3].substr(0, pos)) - 1);

                    f2.uv.push_back(f.uv[0]);
                    f2.uv.push_back(f.uv[2]);
                    f2.uv.push_back(std::stoi(data[3].substr(pos + 1, data[3].length() - pos + 1)) - 1);

                }else
                if(count == 2){
                    int pos1 = data[3].find('/');
                    int pos2 = data[3].rfind('/');
                    f2.verts.push_back(f.verts[0]);
                    f2.verts.push_back(f.verts[2]);                    
                    f2.verts.push_back(std::stoi(data[3].substr(0, pos1)) - 1);
                    if((pos2 - pos1) > 1){
                        f2.uv.push_back(f.uv[0]);
                        f2.uv.push_back(f.uv[2]);
                        f2.uv.push_back(std::stoi(data[3].substr(pos1 + 1, pos2 - pos1 - 1)) - 1);
                    }
                    f2.normals.push_back(f.normals[0]);
                    f2.normals.push_back(f.normals[2]);
                    f2.normals.push_back(std::stoi(data[3].substr(pos2 + 1, data[3].length() - pos2 + 1)) - 1);
                }
                faces.push_back(f2);
            }
        }
        if(!line.compare(0, 2, "g ")){
            iss >> trash;
            iss >> trash;
            groups.push_back(trash);
        }
        if(!line.compare(0, 2, "o ")){
            ++o;
        }
    }
    std::cerr << "\nv  - " << verts.size() << "\nvt - " << uv.size() << "\nvn - "  << normals.size() << "\nf  - "  << faces.size() << std::endl;
    std::cerr << "Objects - " << o << std::endl;
    std::cerr << "Groups - " << groups.size() << std::endl;
    for(unsigned int i = 0; i < groups.size(); ++i)
        std::cerr << i << " - " << groups[i] << std::endl;

    texture.read_tga_file((filename + "/textures/diffuse.tga").c_str());

    z_buffer = new double[size_screen.x * size_screen.y];
    size = Vec3d(max.x - min.x, max.y - min.y, max.z - min.z);
    scale = (size_screen.x / size.x)<(size_screen.y / size.y) ? (size_screen.x / size.x) : (size_screen.y / size.y);
}

Model::~Model() {
    delete[] z_buffer;
}

void Model::draw(HDC hdc){
    for(int i = 0; i < size_screen.x * size_screen.y; ++i) z_buffer[i] = -std::numeric_limits<double>::max();
    // drawMesh(hdc, RGB(255, 255, 255));
    // drawMeshTriangle(hdc);
    drawMeshTexture(hdc);
    // drawZ_buffer(hdc);
}

void Model::drawZ_buffer(HDC hdc){
    for(int j = 0; j < size_screen.y; ++j)
        for(int i = 0; i < size_screen.x; ++i){
            int converted = range(z_buffer[i + j * size_screen.x], (min.z - min.z) * scale, (max.z - min.z) * scale, 0, 255);
            SetPixel(hdc, i, j, RGB(converted, converted, converted));
        }
}

void Model::drawMesh(HDC hdc, const COLORREF &color){
    for (unsigned int i = 0; i < faces.size(); ++i) {
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            Vec3d world_coords = verts[faces[i].verts[j]];
            world_coords = (world_coords - min) * scale;
            screen_coords[j] = Vec2i(world_coords.x, size_screen.y - world_coords.y);
        }
        line(hdc, screen_coords[0], screen_coords[1], color);
        line(hdc, screen_coords[1], screen_coords[2], color);
        line(hdc, screen_coords[2], screen_coords[0], color);
    }
}

void Model::drawMeshTriangle(HDC hdc){
    for (unsigned int i = 0; i < faces.size(); ++i) {
        Vec3d screen_coords[3];
        Vec3d world_coords[3];
        for (int j = 0; j < 3; j++) {
            world_coords[j] = verts[faces[i].verts[j]];
            screen_coords[j] = (world_coords[j] - min) * scale;
            screen_coords[j].y = size_screen.y - screen_coords[j].y;
        }
        Vec3d normal = (world_coords[0] - world_coords[1]) ^ (world_coords[0] - world_coords[2]);
        double intensity = normal.getCosAngle(Vec3d(0.0, 0.0, 1.0));
        if (intensity > 0) {
            int color = range(intensity, 0, 1, 0, 255);
            triangle(hdc, screen_coords, RGB(color, color, color));
        }
    }
}

void Model::drawMeshTexture(HDC hdc){
    for (unsigned int i = 0; i < faces.size(); ++i) {
        Vec3d screen_coords[3];
        Vec3d world_coords[3];
        for (int j = 0; j < 3; j++) {
            world_coords[j] = verts[faces[i].verts[j]];
            screen_coords[j] = (world_coords[j] - min) * scale;
            screen_coords[j].y = size_screen.y - screen_coords[j].y;
        }
        Vec3d normal = (world_coords[0] - world_coords[1]) ^ (world_coords[0] - world_coords[2]);
        double intensity = normal.getCosAngle(Vec3d(0.0, 0.0, 1.0));
        if(intensity > 0)
            triangle(hdc, screen_coords, int(i), intensity);
    }
}
