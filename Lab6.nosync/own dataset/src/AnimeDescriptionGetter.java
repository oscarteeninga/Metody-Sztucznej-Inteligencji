import com.opencsv.CSVWriter;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.File;
import java.io.FileWriter;

public class AnimeDescriptionGetter {
    int count;
    static CSVWriter writer;


    public AnimeDescriptionGetter() throws Exception {
        File file = new File("filmy.csv");
        writer = new CSVWriter(new FileWriter(file), ',',
                CSVWriter.DEFAULT_QUOTE_CHARACTER,
                CSVWriter.DEFAULT_ESCAPE_CHARACTER,
                CSVWriter.DEFAULT_LINE_END);
        writer.writeNext(new String[] {"type", "description"});
        count = 1;
    }

    public String getDescription(String url) {
        try {
            Document doc = Jsoup.connect(url).get();
            String description = doc.select("p").get(1).text();
            description = description.replace("\'", " ");
            description = description.replace(",", " ");
            String type = doc.select("span.second").get(0).text();
            writer.writeNext(new String[] { type, description});
            System.out.println(getCount() + ": " + type + " - " + description);
            increment();
        } catch (Exception e) {
        }
        return null;
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
