class Main {
    static void run() {
        Rectangle.set(8, 6);
        Rectangle.print();
        Cirle.set(12);
        Cirle.print();
        Triangle.set(5,4,3);
        Triangle.print();
        double res = Math.sqrt(0.25);
        ifj16.print(res + "\n");
        return;
    }
}

class Rectangle {
    static double x;
    static double y;
    static void set(double a, double b) {
        x = a;
        y = b;
    }
    static double area() {
        return x*y;
    }
    static double perimetry() {
        return 2*x + 2*y;
    }
    static void print() {
        ifj16.print("Rectangle: " + "\nx = " + x +"\ny = " + y + "\n");
        double a = area();
        double b = perimetry();
        ifj16.print("Area = " + a + "\nPerimetry = " + b + "\n");
    }
}

class Cirle {
    static double r;
    static double pi = 3.14;
    static void set(double a) {
        r = a;
    }
    static double area() {
        return r*r*pi;
    }
    static double perimetry() {
        return 2*r*pi;
    }
    static void print() {
        ifj16.print("Cirle: " + "\nradius = " + r + "\n");
        double a = area();
        double b = perimetry();
        ifj16.print("Area = " + a + "\nPerimetry = " + b + "\n");
    }
}

class Triangle {
    static double x;
    static double y;
    static double z;
    static void set(double a, double b, double c) {
        x = a;
        y = b;
        z = c;
    }
    static double area() {
        double p = perimetry();
        p = p/2;
        p = p*(p-x)*(p-y)*(p-z);
        p = Math.sqrt(p);
        return p;
    }
    static double perimetry() {
        return x+y+z;
    }
    static void print() {
        ifj16.print("Triangle: " + "\nx = " + x +"\ny = " + y + "\nz = " + z + "\n");
        double a = area();
        double b = perimetry();
        ifj16.print("Area = " + a + "\nPerimetry = " + b + "\n");
    }
}

class Math {
    static double sqrt(double x) {
        int i = 25;
        double y = 1;
        while (i > 0) {
            y = 0.5*(x/y+y);
            i = i - 1;
        }
        return y;
    }
}
