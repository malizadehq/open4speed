import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

import javax.imageio.ImageIO;

// storage for materials
class Material {

	ArrayList<Edge> edges;
	ArrayList<Triangle> faces;
	ArrayList<Integer> pointers;
	String parameters;
	static boolean top;
	
	Material(String t) {
		edges = new ArrayList<Edge>();
		faces = new ArrayList<Triangle>();
		pointers = new ArrayList<Integer>();
		parameters = t;
	}

	// get material parameters from library
	static String getMaterial(String lib, String mtl) throws IOException {

		top = false;
		// open material library file
		FileInputStream fis;
		try {
			fis = new FileInputStream(lib);
		} catch (Exception e) {
			System.err.println(lib + " doesn't exists.");
			return "(null)";
		}
		BufferedReader sc = new BufferedReader(new InputStreamReader(fis));

		// set material parameters to zero
		String phong = "";
		// find material in library
		while (sc.ready()) {
			// read one line of library
			String data = sc.readLine();

			// read parameters of material
			if (data.equals("newmtl " + mtl)) {
				// set default variables
				float ar = 0, ag = 0, ab = 0;
				float dr = 0, dg = 0, db = 0;
				float sr = 0, sg = 0, sb = 0;
				float alpha = 1;

				// parse material data
				while (sc.ready()) {
					// read one line of material data
					data = sc.readLine();

					// set ambient parameter
					if (data.startsWith("Ka ")) {
						StringTokenizer lsc = new StringTokenizer(data);
						lsc.nextToken();
						ar = Utils.stringToFloat(lsc.nextToken());
						ag = Utils.stringToFloat(lsc.nextToken());
						ab = Utils.stringToFloat(lsc.nextToken());
					}

					// set diffuse parameter
					if (data.startsWith("Kd ")) {
						StringTokenizer lsc = new StringTokenizer(data);
						lsc.nextToken();
						dr = Utils.stringToFloat(lsc.nextToken());
						dg = Utils.stringToFloat(lsc.nextToken());
						db = Utils.stringToFloat(lsc.nextToken());
					}

					// set specular parameter
					if (data.startsWith("Ks ")) {
						StringTokenizer lsc = new StringTokenizer(data);
						lsc.nextToken();
						sr = Utils.stringToFloat(lsc.nextToken());
						sg = Utils.stringToFloat(lsc.nextToken());
						sb = Utils.stringToFloat(lsc.nextToken());
					}

					// set alpha parameter
					if (data.startsWith("d ")) {
						StringTokenizer lsc = new StringTokenizer(data);
						lsc.nextToken();
						alpha = Utils.stringToFloat(lsc.nextToken());
					}

					// compress parameters
					phong = ar + " " + ag + " " + ab + " ";
					phong += dr + " " + dg + " " + db + " ";
					phong += sr + " " + sg + " " + sb + " ";
					phong += alpha;

					// if transparent place it to the top
					if (alpha < 1) {
						top = true;
					}

					// parse material texture
					if (data.startsWith("map_Kd ")) {

						// create name of processed texture
						int fileStart = data.lastIndexOf('/') + 1;
						if (!data.contains("/")) {
							fileStart = 7;
						}
						String textureFileName = data.substring(fileStart,
								data.length());
						String fileName = data.substring(fileStart,
								data.length());
						// cut extension
						StringTokenizer lsc = new StringTokenizer(
								textureFileName);
						String cut = lsc.nextToken();
						if (cut.contains(".")) {
							cut = cut.substring(0, cut.indexOf('.'));
						}
						textureFileName = ObjConverter.path + "/" + cut + ".png";

						// check if texture is not already processed
						if (!Utils.exists(textureFileName)) {
							// get image of texture
							BufferedImage img;
							BufferedImage resizedImage;
							try {
								if (Utils.exists(data.substring(7))) {
									img = ImageIO.read(new File(data
											.substring(7)));
								} else if (Utils.exists(fileName)) {
									img = ImageIO.read(new File(fileName));
								} else {
									System.err.println("Unable to find "
											+ fileName);
									return "(null)";
								}
								// get type of texture
								int type = img.getType();
								// scale texture
								int w = 2;
								int h = 2;
								while (w < img.getWidth()) {
									w *= 2;
								}
								while (h < img.getHeight()) {
									h *= 2;
								}
								// resize texture
								resizedImage = new BufferedImage(w, h, type);
								Graphics2D g = resizedImage.createGraphics();
								g.drawImage(img, 0, 0, w, h, null);
								g.dispose();
							} catch (Exception e) {
								System.err.println("Error parsing " + fileName);
								fis.close();
								sc.close();
								return "(null)";
							}
							// save resized texture
							try {
								ImageIO.write(resizedImage, "png", new File(
										textureFileName));
								if (!Utils.exists(textureFileName)) {
									System.err
											.println("Unsupported texture format "
													+ fileName);
									fis.close();
									sc.close();
									return "(null)";
								}
							} catch (Exception e) {
								System.err.println("Unable to write "
										+ textureFileName);
								fis.close();
								sc.close();
								return "(null)";
							}
						}
						fis.close();
						sc.close();
						return textureFileName + " " + phong;
					}

					// end of the material
					if (data.startsWith("newmtl ")) {
						break;
					}
				}
			}
		}
		fis.close();
		sc.close();
		return "* " + phong;
	}
}