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

int helpPointPos(Vec2i p, Vec2i a, Vec2i b){
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

bool inTriangle(Vec2i p,  Vec2i* t){
    int q0 = helpPointPos(p, t[0], t[2]);
    int q1 = helpPointPos(p, t[2], t[1]);
    int q2 = helpPointPos(p, t[1], t[0]);
    return (q0 >= 0 && q1 >= 0 && q2 >= 0) || (q0 <= 0 && q1 <= 0 && q2 <= 0);
}

bool Model::helpZ(Vec2i point, Vec3d t0, Vec3d t1, Vec3d t2, double* z_buffer){
    // if(point.x<0 || point.y<0 || point.x>=size_screen.x || point.y>=size_screen.y) return false;
    // Vec3d normal = (t0 - t1) ^ (t0 - t2);
    // double z = (-normal.x * (point.x - t0.x) - normal.y * (point.y - t0.y) + normal.z * t0.z) / normal.z;
    // unsigned long index = point.x + point.y * size_screen.x;
    //     if(z > z_buffer[index]){
    //         z_buffer[index] = z;
    //         return true;
    //     }
    return false;
}

Vec3d barycentric(Vec2i *pts, Vec2i P) {
    Vec3d u = Vec3d(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^ Vec3d(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
    if (std::abs(u.z) < 1) return Vec3d(-1, 1, 1);
    return Vec3d(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

void Model::triangle(HDC hdc, Vec2i* t, const COLORREF &color){
    Vec2i bboxmin(size_screen.x - 1,  size_screen.y - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(size_screen.x - 1, size_screen.y - 1);
    for (int i=0; i<3; i++) {
            bboxmin.x = std::max(0,         std::min(bboxmin.x, t[i].x));
            bboxmax.x = std::min(clamp.x,   std::max(bboxmax.x, t[i].x));
            bboxmin.y = std::max(0,         std::min(bboxmin.y, t[i].y));
            bboxmax.y = std::min(clamp.y,   std::max(bboxmax.y, t[i].y));
    }
    Vec2i P;
    for (P.x = bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3d bc_screen  = barycentric(t, P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            SetPixel(hdc, P.x, P.y, color);
        }
    }

    // Vec2i Min = t[0], Max = t[0];
    // for(int i = 1; i < 3; ++i){
    //     Min.x = t[i].x < Min.x? t[i].x: Min.x; Max.x = t[i].x > Max.x? t[i].x: Max.x;
    //     Min.y = t[i].y < Min.y? t[i].y: Min.y; Max.y = t[i].y > Max.y? t[i].y: Max.y;
    // }

    // for(int j = Min.y; j <= Max.y; ++j){
    //     for(int i = Min.x; i <= Max.x; ++i){
    //         Vec2i point(i, j);
    //             if(inTriangle(point, t))
    //                 // if(helpZ(point, t0, t1, t2, z_buffer))
    //                     SetPixel(hdc, i, j, RGB(255,0,0));
    //     }
    // }
}

Model::Model(const char *filename, Vec2<unsigned long> _size_screen) : verts(), faces(), size_screen(_size_screen) {
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
            std::string data[4];
            for(int i = 0; i < 3; ++i) {
                iss >> data[i];
                data[i].erase(data[i].find('/'), data[i].length() - 1);
                f.push_back(std::stoi(data[i]) - 1);
            }
            faces.push_back(f);
            iss >> data[3];
            if(data[3].length()>0){
                data[3].erase(data[3].find('/'), data[3].length() - 1);
                std::vector<int> f2;
                f2.push_back(std::stoi(data[0]) - 1);
                f2.push_back(std::stoi(data[2]) - 1);
                f2.push_back(std::stoi(data[3]) - 1);
                faces.push_back(f2);
            }
        }
    }
    std::cerr << "# v- " << verts.size() << " f- "  << faces.size() << std::endl;

    z_buffer = new double[size_screen.x * size_screen.y];
    size = Vec3d(max.x - min.x, max.y - min.y, max.z - min.z);
    scale = (size_screen.x < size_screen.y ? size_screen.x : size_screen.y) / (size.x > size.y ? size.x: size.y);
}

Model::~Model() {
    delete[] z_buffer;
}

void Model::draw(HDC hdc){
    for(int i = 0; i < size_screen.x * size_screen.y; ++i) z_buffer[i] = -std::numeric_limits<double>::max();
    // drawMesh(hdc, RGB(255, 255, 255));
    // triangle(hdc, z_buffer, Vec3d(0, 0, 500), Vec3d(0, 800, 500), Vec3d(800, 800, 500), RGB(255, 255, 255));
    // triangle(hdc, z_buffer, Vec3d(0, 0, 500), Vec3d(800, 0, 500), Vec3d(800, 800, 500), RGB(255, 255, 255));
    Vec2i t[3] = {Vec2i(100, 100),Vec2i(600, 300),Vec2i(400, 700)}; 
    // line(hdc, t[0], t[1], RGB(255, 0, 0));
    // line(hdc, t[1], t[2], RGB(255, 0, 0));
    // line(hdc, t[2], t[0], RGB(255, 0, 0));
    triangle(hdc, t, RGB(255, 255, 255));
    // line(hdc, t[0], t[1], RGB(255, 0, 0));
    // line(hdc, t[1], t[2], RGB(255, 0, 0));
    // line(hdc, t[2], t[0], RGB(255, 0, 0));
    // drawMeshTriangle(hdc);
    // drawZ_buffer(hdc);
}

void Model::drawZ_buffer(HDC hdc){
    double old_min = min.z * scale;
    double old_max = max.z * scale;
    double new_min = 0;
    double new_max = 255;

    for(int j = 0; j < size_screen.y; ++j)
        for(int i = 0; i < size_screen.x; ++i){
            double old_range = old_max - old_min;
            double new_range = new_max - new_min;
            int converted = (((z_buffer[i + j * size_screen.x] - old_min) * new_range) / old_range) + new_min;
            if(converted<0) converted = 0;
            SetPixel(hdc, i, j, RGB(converted, converted, converted));
        }
}

void Model::drawMesh(HDC hdc, const COLORREF &color){
    for (unsigned int i = 0; i < faces.size(); ++i) {
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            Vec3d world_coords = verts[faces[i][j]];
            screen_coords[j] = Vec2i((world_coords.x - min.x) * scale, size_screen.y - ((world_coords.y - min.y) * scale));
        }
        line(hdc, screen_coords[0], screen_coords[1], color);
        line(hdc, screen_coords[1], screen_coords[2], color);
        line(hdc, screen_coords[2], screen_coords[0], color);
    }
}

void Model::drawMeshTriangle(HDC hdc){
    // double old_min = 0;
    // double old_max = 1;
    // double new_min = 0;
    // double new_max = 255;

    // for (unsigned int i = 0; i < faces.size(); ++i) {
    //     Vec3d screen_coords[3];
    //     Vec3d world_coords[3];
    //     for (int j = 0; j < 3; j++) {
    //         Vec3d v = verts[faces[i][j]];
    //         screen_coords[j] = Vec3d((v.x - min.x) * scale, size_screen.y - (v.y - min.y) * scale, v.z * scale);
    //         world_coords[j] = v;
    //     }
    //     Vec3d normal = (world_coords[0] - world_coords[1]) ^ (world_coords[0] - world_coords[2]);
    //     double intensity = normal.getCosAngle(Vec3d(0.0, 0.0, 1.0));
    //     if (intensity > 0) {
    //         double old_range = old_max - old_min;
    //         double new_range = new_max - new_min;
    //         int color = (((intensity - old_min) * new_range) / old_range) + new_min;
    //         triangle(hdc, screen_coords[0], screen_coords[1], screen_coords[2], RGB(color, color, color));
    //     }
    // }
}