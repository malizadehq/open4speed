// 3d points connected into edge
class Edge {

	Point3D a, b;

	Edge(Point3D a, Point3D b) {
		this.a = a;
		this.b = b;
	}

	// compressed values of edge
	byte[] value() {
		String output = a.x + " " + a.y + " " + a.z + " ";
		output += b.x + " " + b.y + " " + b.z + "\n";
		return output.getBytes();
	}
}
