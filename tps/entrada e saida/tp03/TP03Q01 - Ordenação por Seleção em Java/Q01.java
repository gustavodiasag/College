import java.io.File;
import java.io.PrintWriter;
import java.util.*;
import java.text.SimpleDateFormat;

class Q01 {

	public static void main(String[] args) throws Exception {

		// Reading from file located at the suggested directory
		Scanner scanner = new Scanner(new File("./tmp/games.csv"));

		// Structure used to store the games into memory
		Game[] games = new Game[4403];

		int pos = 0;

		while (scanner.hasNext()) {

			games[pos] = new Game();

			// Regex is used to separate the line by the commas ignoring the ones inside double quotes
			String[] data = scanner.nextLine().split(",(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
			
			games[pos].setAppId(Integer.parseInt(data[0]));
			games[pos].setName(data[1]);
		
			// Already configuring the dates to the format required in the output
			String formatted = data[2].replace("\"","");
			String date = formatted.substring(0, 3) + "/" + formatted.substring((formatted.length()-4), formatted.length());

			games[pos].setDate((new SimpleDateFormat("MMM/yyyy", Locale.US).parse(date)));

			games[pos].setOwners(data[3]);
			games[pos].setAge(Integer.parseInt(data[4]));
			games[pos].setPrice(Float.parseFloat(data[5]));
			games[pos].setDlcs(Integer.parseInt(data[6]));

			// Splitting and unformatting languages (e.g. "['Portuguese']" -> Portuguese)
			games[pos].setLanguages(data[7].split("(\"\\[')|(', ')|('\\]\")"));

			games[pos].setWebsite(data[8]);
			games[pos].setWindows(Boolean.valueOf(data[9]));
			games[pos].setMac(Boolean.valueOf(data[10]));
			games[pos].setLinux(Boolean.valueOf(data[11]));

			// Upvotes ratio related to the total number of reviews
			games[pos].setUpvotes(Float.parseFloat(data[12]) / (Float.parseFloat(data[12]) + Float.parseFloat(data[13])));

			games[pos].setAvgPt(Integer.parseInt(data[14]));
			games[pos].setDevelopers(data[15]);

			// Dealing with the games that dont have any genre
			games[pos].setGenres((data.length > 16) ? data[16].split(",") : null);
			
			pos++;
		}

		scanner.close();
		
		// Stores the games specified in the first part
		StaticList list = new StaticList(80);

		String input = "";

		// Transferring to the list
		while (!endOfTest(input = MyIO.readLine())) list.insertLast(getGame(games, Integer.parseInt(input)));
		
		long start = (new Date()).getTime(), time;

		list.sort();

		time = (new Date()).getTime() - start;
		
		sendToFile(time, list.comparisons, list.movements);

		list.show();
	}

	// Standard function to check the end of the input
	public static boolean endOfTest(String input) {

		return (input.length() == 3 && input.charAt(0) == 'F' && input.charAt(1) == 'I' && input.charAt(2) == 'M');
	}

	public static Game getGame(Game[] games, int appId) {

		for (int i = 0; i < games.length; i++) if (games[i].getAppId() == appId) return games[i];

		return null;
	}

	public static void sendToFile(long time, int comparisons, int movements) {

		try {

			PrintWriter writer = new PrintWriter("matrícula_selecao.txt");

			writer.print("780888\t");
			writer.print("\t" + time + "ms\t");
			writer.print("\t" + comparisons + "\t");
			writer.print("\t" + movements + "");
			writer.close();

		} catch (Exception e) { System.err.println(e.getMessage()); }
	}
}

class StaticList {

	private Game[] games;
	public int elements, comparisons, movements;

	StaticList() { this(5); }

	StaticList(int size) { this.games = new Game[size]; elements = comparisons = movements = 0; }


	public void insert(int index, Game game) {

		for (int i = elements; i >= index; i--) this.games[i + 1] = this.games[i];

		this.games[index] = game;

		++elements;
	}

	public void insertFirst(Game game) {
	
		for (int i = elements; i >= 0; i--) this.games[i + 1] = this.games[i];

		this.games[0] = game;

		++elements;
	}

	public void insertLast(Game game) { this.games[elements++] = game; }

	public Game remove(int index) {

		Game removed = this.games[index];

		this.games[index] = null;

		for (int i = index; i < elements - 1; i++) this.games[i] = this.games[i + 1];

		--elements;
		
		System.out.println("(R) " + removed.getName());

		return removed;
	}

	public Game removeFirst() {

		Game removed = this.games[0];

		this.games[0] = null;
		
		for (int i = 0; i < elements - 1; i++) this.games[i] = this.games[i + 1];

		--elements;

		System.out.println("(R) " + removed.getName());

		return removed;
	}

	public Game removeLast() {

		Game removed = this.games[--elements];

		this.games[elements] = null;
		
		System.out.println("(R) " + removed.getName());

		return removed;
	}

	public void sort() {

		for (int i = 0; i < elements - 1; i++) {

			int lowest = i;

			for (int j = i + 1; j < elements; j++) {
				
				this.comparisons++;

				if (this.games[lowest].getName().compareTo(this.games[j].getName()) >= 1) lowest = j;
			}

			Game tmp = this.games[lowest];

			this.games[lowest] = this.games[i];

			this.games[i] = tmp;

			this.movements += 3;
		}
	}

	public void show() {

		for (int i = 0; i < elements; i++) games[i].print();
	}	
}

class Game {

	private int app_id, age, dlcs, avg_pt;
	private Date release_date;
	private String name, owners, website, developers;
	private String[] languages, genres;
	private float price, upvotes;
	private boolean windows, mac, linux;

	SimpleDateFormat formatter = new SimpleDateFormat("MMM/yyyy", Locale.US);
	
	// Constructors
	Game() { this(0, 0, 0, 0, null, "", "", "", "", null, null, 0, 0, false, false, false); }

	Game(int app_id, int age, int dlcs, int avg_pt, Date release_date, String name, String owners, String website, String developers
	     , String[] languages, String[] genres, float price, float upvotes, boolean windows, boolean mac, boolean linux) {

		this.app_id = app_id;
		this.age = age;
		this.dlcs = dlcs;
		this.release_date = release_date;
		this.name = name;
		this.owners = owners; 
		this.website = website;
		this.developers = developers;
		this.languages = languages;
		this.genres = genres;
		this.windows = windows;
		this.mac = mac;
		this.linux = linux;

	}

	// Set functions for each private attribute
	void setAppId(int app_id) { this.app_id = app_id; }
	void setAge(int age) { this.age = age; }
	void setDlcs(int dlcs) { this.dlcs = dlcs; }
	void setAvgPt(int avg_pt) { this.avg_pt = avg_pt; }
	void setDate(Date release_date) { this.release_date = release_date; }
	void setName(String name) { this.name = name; }
	void setOwners(String owners) { this.owners = owners; }
	void setWebsite(String website) { this.website = website; }
	void setDevelopers(String developers) { this.developers = developers; }
	void setLanguages(String[] languages) { this.languages = languages; }
	void setGenres(String[] genres) { this.genres = genres; }
	void setPrice(float price) { this.price = price; }
	void setUpvotes(float upvotes) { this.upvotes = upvotes; }
	void setWindows(boolean windows) { this.windows = windows; }
	void setMac(boolean mac) { this.mac = mac; }
	void setLinux(boolean linux) { this.linux = linux; }

	// Get functions for each private attribute
	int getAppId() { return this.app_id; }
	int getAge() { return this.age;}
	int getDlcs() { return this.dlcs; }
	int getAvgPt() { return this.avg_pt; }
	String getDate() { return formatter.format(this.release_date); }
	String getName() { return this.name;} 
	String getOwners() { return this.owners; }
	String getWebsite() { return this.website; }
	String getDevelopers() { return this.developers; }
	String[] getLanguages() { return this.languages; }
	String[] getGenres() { return this.genres; }
	float getPrice()  { return this.price; }
	float getUpvotes() { return this.upvotes; }
	boolean getWindows() { return this.windows; }
	boolean getMac() { return this.mac; }
	boolean getLinux() { return this.linux; }

	// Creates a new Game object in memory
	public Game clone() {

		Game clone = new Game();

		clone.setAppId(this.app_id);
		clone.setAge(this.age);
		clone.setDlcs(this.dlcs);
		clone.setAvgPt(this.avg_pt);
		clone.setDate(this.release_date);
		clone.setName(this.name);
		clone.setOwners(this.owners);
		clone.setWebsite(this.website);
		clone.setDevelopers(this.developers);
		clone.setLanguages(this.languages);
		clone.setGenres(this.genres);
		clone.setPrice(this.price);
		clone.setUpvotes(this.upvotes);
		clone.setWindows(this.windows);
		clone.setMac(this.mac);
		clone.setLinux(this.linux);

		return clone;
	}

	// Used to apply a dot as a decimal separator instead of a comma
	void printPrice() { System.out.format(Locale.US, "%.2f ", this.price); }

	void printLanguages() {

		System.out.print("[");

		if (languages.length == 1) System.out.print(languages[0].replaceAll("[\\[\\]']", "") + "] ");

		else {

			for (int i = 1; i < languages.length-1; i++) System.out.print(languages[i] + ", ");

			System.out.print(languages[languages.length-1] + "] ");
		}
	}
	
	// Output requires to show "null" when there's no website
	void printWebsite() { System.out.print((this.website.isEmpty()) ? "null " : this.website + " "); }

	// Converting the ratio to a more proper percentage format
	void printUpvotes() { System.out.print((int)Math.round(this.upvotes * 100) + "% "); }

	// Dealing with all possible cases
	void printAvgPt() {

		int hours = this.avg_pt / 60, minutes = this.avg_pt % 60;

		if (hours == 0 && minutes == 0) System.out.print("null ");
		
		else System.out.print(((hours > 0) ? hours + "h " : "") + ((minutes > 0) ? minutes + "m " : ""));
	}

	void printGenres() {

		System.out.print("[");

		if (genres.length == 1) System.out.print(genres[0].replace("\"", "") + "]\n");

		else {

			System.out.print(genres[0].replace("\"", "") + ", ");

			for (int i = 1; i < genres.length-1; i++) System.out.print(genres[i] + ", ");

			System.out.print(genres[genres.length-1].replace("\"", "") + "]\n");
		}
	}

	// Shows all informations properly
	void print() {
			
		System.out.printf("%d %s %s %s %d ", this.app_id, this.name, formatter.format(this.release_date), this.owners, this.age);

		printPrice();

		System.out.print(this.dlcs + " ");

		printLanguages();
		printWebsite();
		
		System.out.printf("%b %b %b ", this.windows, this.mac, this.linux);

		printUpvotes();
		printAvgPt();

		System.out.print(this.developers.replace("\"", "") + " ");

		printGenres();
	}
}
