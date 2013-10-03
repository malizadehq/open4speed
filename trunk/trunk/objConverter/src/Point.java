// unit for store full 3d point
class Point {

	Point2D t;
	Point3D v, n;

	Point(Point3D n, Point2D t, Point3D v) {
		this.n = n;
		this.t = t;
		this.v = v;
	}

	// compressed values of point
	String value() {
		String output = "";
		if (t == null) {
			output += "0 0 ";
			ObjConverter.missingCoord++;
		} else {
			output += t.u + " " + t.v + " ";
		}

		if (n == null) {
			output += "0 0 0 ";
			ObjConverter.missingNormal++;
		} else {
			output += n.x + " " + n.y + " " + n.z + " ";
		}

		return output + v.x + " " + v.y + " " + v.z + " ";
	}
}