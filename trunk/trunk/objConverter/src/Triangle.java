// 3d points connected into triangle
class Triangle {

	Point a, b, c;
	int cell;

	Triangle(Point a, Point b, Point c) {
		this.a = a;
		this.b = b;
		this.c = c;
	}

	// compressed values of triangle
	byte[] value() {
		String output = a.value() + b.value() + c.value() + "\n";
		return output.getBytes();
	}
}