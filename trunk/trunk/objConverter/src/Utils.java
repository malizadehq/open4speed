import java.io.FileInputStream;


public class Utils {

	// check if file exists
	static boolean exists(String filename) {
		try {
			FileInputStream fis = new FileInputStream(filename);
			if (fis.available() > 0) {
				fis.close();
				return true;
			} else {
				fis.close();
				return false;
			}
		} catch (Exception e) {
			return false;
		}
	}

	static int getCell(Point p) {
		int x = (int) ((p.v.x - ObjConverter.minx) / ObjConverter.cellSize);
		int z = (int) ((p.v.z - ObjConverter.minz) / ObjConverter.cellSize);
		return z * ObjConverter.cellWidth + x;
	}
	
	// convert string to float
	static float stringToFloat(String value) {
		try {
			return Float.valueOf(value);
		} catch (Exception e) {
			return 0;
		}
	}
}
