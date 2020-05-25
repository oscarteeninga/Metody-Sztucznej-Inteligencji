import com.opencsv.CSVWriter;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.File;
import java.io.FileWriter;

public class AnimeDescriptionGetter {
    int count;
    int documentary;
    int anime = 0;
    static CSVWriter writer;

    public AnimeDescriptionGetter() throws Exception {
        File file = new File("filmy.csv");
        writer = new CSVWriter(new FileWriter(file), ',',
                CSVWriter.DEFAULT_QUOTE_CHARACTER,
                CSVWriter.DEFAULT_ESCAPE_CHARACTER,
                CSVWriter.DEFAULT_LINE_END);
        writer.writeNext(new String[] {"type", "description"});
        count = 1;
        documentary = 1;
        anime = 1;
    }

    public int getDocumentary(String url) {
        try {
            Document doc = Jsoup.connect(url).get();
            String description = doc.select("p").get(1).text();
            description = description.replace("\"", " ");
            description = description.replace(",", " ");
            String type = doc.select("span.second").get(0).text();
            if (type.equals("Film dokumentalny")) {
                writer.writeNext(new String[] { type, description});
                System.out.println(getCount() + ": " + type + " - " + description);
                increment();
                documentary++;
            }
        } catch (Exception e) {

        } finally {
            return documentary;
        }
    }

    public int getAnime(String url) {
        try {
            Document doc = Jsoup.connect(url).get();
            String description = doc.select("p").get(0).text();
            description = description.replace("\"", " ");
            description = description.replace(",", " ");
            if (!description.equals("Główna") && !description.equals("Sorry_no_characters") && !description.equals("Drugoplanowa") && !description.equals("")) {
                writer.writeNext(new String[] { "Anime", description});
                System.out.println(getCount() + ": Anime - " + description);
                increment();
                anime++;
            }
        } catch (Exception e) {
        } finally {
            return anime;
        }
    }

    public void close() throws Exception {
        writer.close();
    }

    synchronized void increment()  {
        count++;
    }

    synchronized int getCount()  {
        return count;
    }
}
