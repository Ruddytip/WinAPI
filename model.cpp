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
    if (x0>x1) {
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

int helpPointPos(Vec2i p, Vec2i a, Vec2i b){
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

bool inTriangle(Vec2i p,  Vec2i t0, Vec2i t1, Vec2i t2){
    int q0 = helpPointPos(p, t0, t1);
    int q1 = helpPointPos(p, t1, t2);
    int q2 = helpPointPos(p, t2, t0);
    return (q0>=0 && q1>=0 && q2>=0) || (q0<=0 && q1<=0 && q2<=0);
}

void Model::triangle(HDC hdc, int* z_buffer, Vec2i t0, Vec2i t1, Vec2i t2, const COLORREF &color){
    if (t0.y == t1.y && t0.y == t2.y) return;
    if (t0.x == t1.x && t0.x == t2.x) return;

    if(t0.y > t1.y) std::swap(t0, t1);
    if(t0.y > t2.y) std::swap(t0, t2);
    if(t1.y > t2.y) std::swap(t1, t2);

    Vec2i Min = t0, Max = t0;
    Min.x = t1.x < Min.x? t1.x: Min.x; Max.x = t1.x > Max.x? t1.x: Max.x;
    Min.y = t1.y < Min.y? t1.y: Min.y; Max.y = t1.y > Max.y? t1.y: Max.y;
    Min.x = t2.x < Min.x? t2.x: Min.x; Max.x = t2.x > Max.x? t2.x: Max.x;
    Min.y = t2.y < Min.y? t2.y: Min.y; Max.y = t2.y > Max.y? t2.y: Max.y;

    for(int j = Min.y; j <= Max.y; ++j){
        for(int i = Min.x; i <= Max.x; ++i){
            if(inTriangle(Vec2i(i, j), t0, t1, t2)) SetPixel(hdc, i, j, color);
        }
    }
}

Model::Model(const char *filename, int Size) : verts(), faces(), size(Size) {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
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
        if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            std::vector<int> f;
            std::string data[3];
            for(int i = 0; i < 3; ++i) {
                iss >> data[i];
                data[i].erase(data[i].find('/'), data[i].length() - 1);
                f.push_back(std::stoi(data[i]) - 1);
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v- " << verts.size() << " f- "  << faces.size() << std::endl;
}

Model::~Model() {
}

void Model::draw(HDC hdc){
    drawMeshTriangle(hdc);
}

void Model::drawMesh(HDC hdc, const COLORREF &color){
    double x = max.x - min.x;
    double y = max.y - min.y;
    double scale = size / (x > y ? x: y);
    double dx = - min.x;
    double dy = - min.y;
    for (unsigned int i = 0; i < faces.size(); ++i) {
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            Vec3d world_coords = verts[faces[i][j]];
            screen_coords[j] = Vec2i((world_coords.x + dx) * scale, size - ((world_coords.y + dy) * scale));
        }
        line(hdc, screen_coords[0], screen_coords[1], color);
        line(hdc, screen_coords[1], screen_coords[2], color);
        line(hdc, screen_coords[2], screen_coords[0], color);
    }
}

void Model::drawMeshTriangle(HDC hdc){
    double x = max.x - min.x;
    double y = max.y - min.y;
    double scale = size / (x > y ? x: y);
    double dx = - min.x;
    double dy = - min.y;
    int* z_buffer = new int[size * size];
    for (long i = 0; i < size * size; ++i) z_buffer[i] = std::numeric_limits<int>::min();
    for (unsigned int i = 0; i < faces.size(); i++) {
        Vec2i screen_coords[3];
        Vec3d world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3d v = verts[faces[i][j]];
            screen_coords[j] = Vec2d((v.x + dx) * scale, size - (v.y + dy) * scale);
            world_coords[j]  = v;
        }
        Vec3d n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
        double intensity = n.getCosAngle(Vec3d(0, 0, 1));
        if (intensity > 0) {
            triangle(hdc, z_buffer, screen_coords[0], screen_coords[1], screen_coords[2], RGB(intensity * 255, intensity * 255, intensity * 255));
        }
    }
    delete[] z_buffer;
}