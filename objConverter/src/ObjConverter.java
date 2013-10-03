import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * Tool for converting OBJ models into O4S
 * 
 * @author Lubos Vonasek
 */
public class ObjConverter {

	static final int cellSize = 300;
	static int cellWidth;
	static int cellHeight;
	static int missingCoord = 0;
	static int missingNormal = 0;
	static String path;


	// variables to count extreme coordinates of model
	static float minx = Float.MAX_VALUE;
	static float miny = Float.MAX_VALUE;
	static float minz = Float.MAX_VALUE;
	static float maxx = Float.MIN_VALUE;
	static float maxy = Float.MIN_VALUE;
	static float maxz = Float.MIN_VALUE;
	
	/**
	 * Program code
	 * 
	 * @param args
	 *            <input.obj> <xxx/yyy.o4s>
	 * @throws IOException
	 */
	public static void main(String[] args) throws IOException {

		/*
		 * args = new String[2]; args[0] = "bin/room.obj"; args[1] =
		 * "bin/tracks/room.o4s";
		 */

		// create time stamp
		long timestamp = System.currentTimeMillis();

		// header
		String[] text = {
				" ___________________________________________________________",
				"|                                                           |",
				"|            Open4speed by L.Vonásek OBJ converter          |",
				"|                      version 1.00                         |",
				"|___________________________________________________________|",
				"     usage: java ObjConverter <input.obj> <xxx/yyy.o4s>      ",
				"" };
		for (int i = 0; i < text.length; i++) {
			System.out.println(text[i]);
		}

		// check arguments
		if (args.length != 2) {
			System.err.println("Invalid arguments");
			return;
		}

		// Unix absolute path report
		if ((args[0].charAt(0) == '/') | (args[1].charAt(0) == '/')) {
			System.err.println("Do not use absoulute path");
			return;
		}

		// windows absolute path report
		if ((args[1].charAt(1) == ':') | (args[1].charAt(1) == ':')) {
			System.err.println("Do not use absoulute path");
			return;
		}

		// get path of output directory
		try {
			path = args[1].substring(0, args[1].lastIndexOf('/'));
			// backup old directory
			new File(path).renameTo(new File("backup-"
					+ System.currentTimeMillis()));
		} catch (Exception e) {
			System.err
					.println("Do not put output file into converter directory");
			return;
		}
		// create output directory
		new File(path).mkdirs();

		// open input file
		FileInputStream fis;
		try {
			fis = new FileInputStream(args[0]);
		} catch (Exception ex) {
			System.err.println("Unable to open input file");
			return;
		}
		BufferedReader sc = new BufferedReader(new InputStreamReader(fis));

		// create storage in memory for OBJ model
		ArrayList<Point3D> vertices = new ArrayList<Point3D>();
		ArrayList<Point3D> normals = new ArrayList<Point3D>();
		ArrayList<Point2D> texCoords = new ArrayList<Point2D>();
		ArrayList<Material> materials = new ArrayList<Material>();

		// for broken faces create null material
		materials.add(new Material("(null)"));
		// set link for saving faces
		ArrayList<Edge> edges = materials.get(0).edges;
		ArrayList<Triangle> faces = materials.get(0).faces;
		String prefix = " ";

		/**
		 * Loading OBJ file
		 */
		System.out.print("Loading file into memory...");
		String currentMaterialLib = "";
		while (sc.ready()) {
			// read one line of file
			String line = sc.readLine();

			// set prefix of material
			if (line.startsWith("o ")) {
				prefix = line.substring(1);
				if (!prefix.contains("#") && !prefix.contains("$")
						&& !prefix.contains("%") && !prefix.contains("!")) {
					prefix = " ";
				}
			}

			// set material library
			if (line.startsWith("mtllib ")) {
				String mtlPath = args[0].substring(0,
						args[0].lastIndexOf('/') + 1);
				currentMaterialLib = mtlPath + line.substring(7);
			}

			// change current material
			if (line.startsWith("usemtl ")) {
				// get parameters of material
				String material = Material.getMaterial(currentMaterialLib,
						line.substring(7));
				material += prefix;
				// check if material already exists
				boolean assigned = false;
				for (int i = 0; i < materials.size(); i++) {
					if (materials.get(i).parameters.compareTo(material) == 0) {
						edges = materials.get(i).edges;
						faces = materials.get(i).faces;
						assigned = true;
					}
				}
				// if material doesn't exist then create it
				if (!assigned) {
					if (Material.top) {
						materials.add(new Material(material));
					} else {
						materials.add(1, new Material(material));
					}
					edges = materials.get(materials.size() - 1).edges;
					faces = materials.get(materials.size() - 1).faces;
				}
			}

			// add vertex into memory
			if (line.startsWith("v ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// get values
				float x = Utils.stringToFloat(lsc.nextToken());
				float y = Utils.stringToFloat(lsc.nextToken());
				float z = Utils.stringToFloat(lsc.nextToken());
				// update extreme values
				if (x < minx) {
					minx = x;
				}
				if (y < miny) {
					miny = y;
				}
				if (z < minz) {
					minz = z;
				}
				if (x > maxx) {
					maxx = x;
				}
				if (y > maxy) {
					maxy = y;
				}
				if (z > maxz) {
					maxz = z;
				}
				// add it into memory
				vertices.add(new Point3D(x, y, z));
			}

			// add normal into memory
			if (line.startsWith("vn ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// get values
				float x = Utils.stringToFloat(lsc.nextToken());
				float y = Utils.stringToFloat(lsc.nextToken());
				float z = Utils.stringToFloat(lsc.nextToken());
				// add it into memory
				normals.add(new Point3D(x, y, z));
			}

			// add texture coordinate into memory
			if (line.startsWith("vt ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// get values
				float x = Utils.stringToFloat(lsc.nextToken());
				float y = Utils.stringToFloat(lsc.nextToken());
				// add it into memory
				texCoords.add(new Point2D(x, y));
			}
		}
		System.out.println("OK");

		/**
		 * OBJ file parsing
		 */
		try {
			fis.close();
			fis = new FileInputStream(args[0]);
		} catch (Exception ex) {
			System.err.println("Unable to open input file");
			return;
		}
		sc = new BufferedReader(new InputStreamReader(fis));
		System.out.print("Parsing OBJ file...");

		// parse all faces
		while (sc.ready()) {
			// read one line of file
			String line = sc.readLine();

			// set prefix of material
			if (line.startsWith("o ")) {
				prefix = line.substring(1);
				if (!prefix.contains("#") && !prefix.contains("$")
						&& !prefix.contains("%") && !prefix.contains("!")) {
					prefix = " ";
				}
			}

			// set material library
			if (line.startsWith("mtllib ")) {
				String mtlPath = args[0].substring(0,
						args[0].lastIndexOf('/') + 1);
				currentMaterialLib = mtlPath + line.substring(7);
			}

			// change current material
			if (line.startsWith("usemtl ")) {
				// get parameters of material
				String material = Material.getMaterial(currentMaterialLib,
						line.substring(7));
				material += prefix;
				// check if material already exists
				boolean assigned = false;
				for (int i = 0; i < materials.size(); i++) {
					if (materials.get(i).parameters.compareTo(material) == 0) {
						edges = materials.get(i).edges;
						faces = materials.get(i).faces;
						assigned = true;
					}
				}
				// if material doesn't exist then create it
				if (!assigned) {
					if (Material.top) {
						materials.add(new Material(material));
					} else {
						materials.add(1, new Material(material));
					}
					edges = materials.get(materials.size() - 1).edges;
					faces = materials.get(materials.size() - 1).faces;
				}
			}

			// add edge into memory
			if (line.startsWith("l ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// create object for points
				Point[] point = new Point[4];
				// set index of current point
				int pointsCount = 0;
				// parse vertices
				while (lsc.hasMoreTokens()) {
					// get point as scanner
					StringTokenizer vsc = new StringTokenizer(lsc.nextToken()
							.replace('/', ' '));

					// convert value into point
					Point3D vertex = vertices.get(Integer.valueOf(vsc
							.nextToken()) - 1);
					Point2D texCoord = null;
					if (vsc.hasMoreTokens() & (texCoords.size() > 0)) {
						int index = Integer.valueOf(vsc.nextToken()) - 1;
						if (index < texCoords.size()) {
							texCoord = texCoords.get(index);
						}
					}
					Point3D normal = null;
					if (vsc.hasMoreTokens() & (normals.size() > 0)) {
						normal = normals
								.get(Integer.valueOf(vsc.nextToken()) - 1);
					}

					// save point
					point[pointsCount++] = new Point(normal, texCoord, vertex);
				}
				if (pointsCount == 2) {
					edges.add(new Edge(point[0].v, point[1].v));
				}
			}
			
			// add face into memory
			if (line.startsWith("f ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// create object for points
				Point[] point = new Point[4];
				// set index of current point
				int pointsCount = 0;
				// parse vertices
				while (lsc.hasMoreTokens()) {
					// get point as scanner
					StringTokenizer vsc = new StringTokenizer(lsc.nextToken()
							.replace('/', ' '));

					// convert value into point
					Point3D vertex = vertices.get(Integer.valueOf(vsc
							.nextToken()) - 1);
					Point2D texCoord = null;
					if (vsc.hasMoreTokens() & (texCoords.size() > 0)) {
						int index = Integer.valueOf(vsc.nextToken()) - 1;
						if (index < texCoords.size()) {
							texCoord = texCoords.get(index);
						}
					}
					Point3D normal = null;
					if (vsc.hasMoreTokens() & (normals.size() > 0)) {
						normal = normals
								.get(Integer.valueOf(vsc.nextToken()) - 1);
					}

					// save point
					point[pointsCount++] = new Point(normal, texCoord, vertex);
				}

				switch (pointsCount) {
				case (2):
					edges.add(new Edge(point[0].v, point[1].v));
					break;
				// save triangle
				case (3):
					faces.add(new Triangle(point[0], point[1], point[2]));
					break;
				// save polygons(separate it to two triangles)
				case (4):
					faces.add(new Triangle(point[3], point[0], point[1]));
					faces.add(new Triangle(point[3], point[1], point[2]));
					break;
				}
			}
		}
		System.out.println("OK");

		System.out.print("Subdividing into cells...");
		// subdivide
		cellWidth = (int) ((maxx - minx) / cellSize + 1);
		cellHeight = (int) ((maxz - minz) / cellSize + 1);
		if (cellWidth * cellHeight <= 1) {
			cellWidth = 1;
			cellHeight = 1;
		} else {
			cellHeight++;
		}
		for (int j = 1; j < materials.size(); j++) {
			faces = materials.get(j).faces;
			for (int i = 0; i < faces.size(); i++) {
				int a = Utils.getCell(faces.get(i).a);
				int b = Utils.getCell(faces.get(i).b);
				int c = Utils.getCell(faces.get(i).c);
				faces.get(i).cell = a;
				if ((a != b) || (a != c) || (b != c)) {
					faces.get(i).cell = cellWidth * cellHeight - 1;
				}
			}
			
			// sort
			ArrayList<Triangle> triangles = new ArrayList<Triangle>();
			for (int l = 0; l <= cellWidth * cellHeight; l++) {
				for (int i = 0; i < faces.size(); i++) {
					if (faces.get(i).cell == l) {
						triangles.add(faces.get(i));
						faces.remove(i);
						i--;
					}
				}
			}
			materials.get(j).faces = triangles;
			faces = triangles;
			
			// find pointers
			int index = 0;
			for (int i = 1; i < cellWidth * cellHeight; i++) {
				if (index < faces.size()) {
					while (faces.get(index).cell < i) {
						if (index == faces.size() - 1) {
							break;
						}
						index++;
					}
				}
				materials.get(j).pointers.add(index);
			}
		}
		System.out.println("OK");

		// count faces and edges
		int edgesCount = 0;
		int facesCount = 0;
		for (int j = 1; j < materials.size(); j++) {
			// check if current material has edges and faces
			if (materials.get(j).edges.size() > 0) {
				edgesCount++;
			}
			if (materials.get(j).faces.size() > 0) {
				facesCount++;
			}
		}

		try {
			// open output file
			FileOutputStream fos = new FileOutputStream(args[1]);
			System.out.print("Writing output file...");

			// save cells size
			fos.write((cellWidth + "\n").getBytes());
			fos.write((cellHeight + "\n").getBytes());

			// save extreme values
			String extremes = minx + " " + miny + " " + minz + " ";
			extremes += maxx + " " + maxy + " " + maxz + "\n";
			fos.write((extremes).getBytes());

			// save faces count
			fos.write((facesCount + "\n").getBytes());

			// save all faces
			for (int j = 1; j < materials.size(); j++) {
				// get faces pointer for current material
				faces = materials.get(j).faces;
				// find local center
				minx = 99999;
				miny = 99999;
				minz = 99999;
				maxx = -99999;
				maxy = -99999;
				maxz = -99999;
				for (int i = 0; i < faces.size(); i++) {
					if (minx > faces.get(i).a.v.x)
						minx = faces.get(i).a.v.x;
					if (miny > faces.get(i).a.v.y)
						miny = faces.get(i).a.v.y;
					if (minz > faces.get(i).a.v.z)
						minz = faces.get(i).a.v.z;

					if (maxx < faces.get(i).a.v.x)
						maxx = faces.get(i).a.v.x;
					if (maxy < faces.get(i).a.v.y)
						maxy = faces.get(i).a.v.y;
					if (maxz < faces.get(i).a.v.z)
						maxz = faces.get(i).a.v.z;

					if (minx > faces.get(i).b.v.x)
						minx = faces.get(i).b.v.x;
					if (miny > faces.get(i).b.v.y)
						miny = faces.get(i).b.v.y;
					if (minz > faces.get(i).b.v.z)
						minz = faces.get(i).b.v.z;

					if (maxx < faces.get(i).b.v.x)
						maxx = faces.get(i).b.v.x;
					if (maxy < faces.get(i).b.v.y)
						maxy = faces.get(i).b.v.y;
					if (maxz < faces.get(i).b.v.z)
						maxz = faces.get(i).b.v.z;

					if (minx > faces.get(i).c.v.x)
						minx = faces.get(i).c.v.x;
					if (miny > faces.get(i).c.v.y)
						miny = faces.get(i).c.v.y;
					if (minz > faces.get(i).c.v.z)
						minz = faces.get(i).c.v.z;

					if (maxx < faces.get(i).c.v.x)
						maxx = faces.get(i).c.v.x;
					if (maxy < faces.get(i).c.v.y)
						maxy = faces.get(i).c.v.y;
					if (maxz < faces.get(i).c.v.z)
						maxz = faces.get(i).c.v.z;
				}

				// check if current material has faces
				if (faces.size() > 0) {
					// save material parameters
					fos.write((minx + " " + miny + " " + minz + " " + maxx
							+ " " + maxy + " " + maxz + " "
							+ materials.get(j).parameters + "\n").getBytes());
					// save faces count
					for (int l = 0; l < materials.get(j).pointers.size(); l++) {
						fos.write((materials.get(j).pointers.get(l) + "\n")
								.getBytes());
					}
					fos.write((faces.size() + "\n").getBytes());
					// save face parameters
					for (int i = 0; i < faces.size(); i++) {
						faces.get(i).a.v.x -= minx;
						faces.get(i).a.v.y -= miny;
						faces.get(i).a.v.z -= minz;
						faces.get(i).b.v.x -= minx;
						faces.get(i).b.v.y -= miny;
						faces.get(i).b.v.z -= minz;
						faces.get(i).c.v.x -= minx;
						faces.get(i).c.v.y -= miny;
						faces.get(i).c.v.z -= minz;
						fos.write(faces.get(i).value());
						faces.get(i).a.v.x += minx;
						faces.get(i).a.v.y += miny;
						faces.get(i).a.v.z += minz;
						faces.get(i).b.v.x += minx;
						faces.get(i).b.v.y += miny;
						faces.get(i).b.v.z += minz;
						faces.get(i).c.v.x += minx;
						faces.get(i).c.v.y += miny;
						faces.get(i).c.v.z += minz;
					}
				}
			}

			// save edges count
			fos.write((edgesCount + "\n").getBytes());

			// save all edges
			for (int j = 0; j < materials.size(); j++) {
				// get edges pointer for current material
				edges = materials.get(j).edges;
				// check if current material has edges
				if (edges.size() > 0) {
					// save edges count
					fos.write((edges.size() + "\n").getBytes());
					// save face parameters
					for (int i = 0; i < edges.size(); i++) {
						fos.write(edges.get(i).value());
					}
				}
			}

			// close files
			fos.close();
			fis.close();
			System.out.println("OK");

		} catch (IOException e) {
			System.err.println("Unable to save file");
			return;
		}

		// show information
		if (materials.get(0).faces.size() > 0) {
			System.out.println(materials.get(0).faces.size()
					+ " polygons weren't parsed.");
		}
		if (missingNormal > 0) {
			System.out.println(missingNormal + " polygons haven't normal.");
		}
		if (missingCoord > 0) {
			System.out.println(missingCoord
					+ " vertices haven't texture coords.");
		}

		if (facesCount > 0) {
			System.out.println("Converted " + facesCount + " triangle objects");
			if (edgesCount > 0) {
				System.out.println("Converted " + edgesCount + " edge tracks");
			}
			int time = (int) ((System.currentTimeMillis() - timestamp) / 1000);
			System.out.println("Done in " + time + " seconds.");
		} else {
			System.out.println("Nothing converted");
		}
	}
}