#define LOGXY(x, y) std::cout << "Coppia\tX: " << x << "\tY: " << y << std::endl
#define LOGVECTORPUBLIC(vector)  std::cout << "Vettore\tX: " vector.getPosition().x << "\tY: " << vector.getPosition().y << std::endl
#define LOGVECTOR(stringa, vec) std::cout << stringa << "\tX: " << vec.x << "\tY: " << vec.y << std::endl;
#define COLORE(color) \
  color[0] * 0xFF, color[1] * 0xFF, color[2] * 0xFF, color[3] * 0xFF
