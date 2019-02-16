import java.io.*;
import java.security.*;
import java.util.*;

public class CrosswordPuzzlerVis {
    public static void main(String[] args) {
        new CrosswordPuzzler(args);
    }
}

class ErrorReader extends Thread {

    private final InputStream error;

    public ErrorReader(InputStream is) {
        error = is;
    }

    @Override
    public void run() {
        try (Scanner scanner = new Scanner(error)) {
            while (scanner.hasNextLine()) {
                System.err.println(scanner.nextLine());
            }
            System.err.flush();
        }
    }
}

class CrosswordPuzzler {
    InputStream is;
    OutputStream os;
    BufferedReader br;
    long seed = 1;

    public CrosswordPuzzler(String[] args) {
        try {
            String exec = null;
            for (int i = 0; i < args.length; i++) {
                if (args[i].equals("-seed"))
                    seed = Long.parseLong(args[++i]);
                if (args[i].equals("-exec"))
                    exec = args[++i];
            }
            Runtime rt = Runtime.getRuntime();
            Process proc = rt.exec(exec);
            os = proc.getOutputStream();
            is = proc.getInputStream();
            br = new BufferedReader(new InputStreamReader(is));
            new ErrorReader(proc.getErrorStream()).start();
            System.out.println(runTest());
            proc.destroy();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public void addFatalError(String s) {
        System.err.println(seed + " : " + s);
    }

    public String[] createPuzzle(int width, int height, String[] dict) {
        try {
            StringBuffer sb = new StringBuffer();
            sb.append(width).append("\n");
            sb.append(height).append("\n");
            sb.append(dict.length).append("\n");
            for (int i = 0; i < dict.length; i++)
                sb.append(dict[i]).append("\n");
            os.write(sb.toString().getBytes());
            os.flush();
            String[] result = new String[Integer.parseInt(br.readLine())];
            for (int i = 0; i < result.length; i++)
                result[i] = br.readLine();
            return result;
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private int width, height;
    private HashSet<String> wordList = new HashSet<String>();
    private String[] words;
    private int[] scores = new int[] { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144 };

    public int runTest() {
        {
            Random r = null;
            try {
                r = SecureRandom.getInstance("SHA1PRNG");
                r.setSeed(seed);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
            width = r.nextInt(21) + 30;
            height = r.nextInt(21) + 30;
            int numWords = height * width / 4 + r.nextInt(1 + height * width - height * width / 4);
            String[] dist = new String[27];
            for (int i = 0; i < 27; i++) {
                char c = 'A';
                dist[i] = "";
                while (c <= 'Z') {
                    if (r.nextDouble() < 0.6) {
                        dist[i] += c;
                    } else {
                        c++;
                    }
                }
            }
            int[] lenDist = new int[] { 1, 1, 2, 2, 3, 4 };
            while (wordList.size() < numWords) {
                int wordLen = lenDist[r.nextInt(6)] + lenDist[r.nextInt(6)] + lenDist[r.nextInt(6)];
                char lastChar;
                String thisWord = "" + (lastChar = dist[26].charAt(r.nextInt(dist[26].length())));
                while (thisWord.length() < wordLen) {
                    String s = dist[lastChar - 'A'];
                    thisWord += lastChar = s.charAt(r.nextInt(s.length()));
                }
                wordList.add(thisWord);
            }
            words = wordList.toArray(new String[0]);
        }
        String[] ret = createPuzzle(width, height, words);
        if (ret.length != height) {
            addFatalError("Return value was expected to have " + height + " elements.");
            return -1;
        }
        int score = 0;
        for (int i = 0; i < ret.length; i++) {
            if (ret[i].length() != width) {
                addFatalError("Element " + i + " of return was expected to have " + width + " characters.");
                return -1;
            }
            for (int j = 0; j < ret[i].length(); j++) {
                char c = ret[i].charAt(j);
                if (c == '-') {
                    score--;
                    continue;
                }
                if (c < 'A' || c > 'Z') {
                    addFatalError("Character " + j + " of element " + i + " of return is not a valid character.");
                    return -1;
                }
            }
        }
        for (int i = 0; i < ret.length; i++) {
            String[] s = ret[i].split("-");
            for (int j = 0; j < s.length; j++) {
                if (s[j].length() > 1) {
                    if (wordList.contains(s[j])) {
                        wordList.remove(s[j]);
                        score += scores[s[j].length()];
                    } else {
                        addFatalError("Horizontal word " + s[j] + " is not in the dictionary or is duplicated.");
                        return -1;
                    }
                }
            }
        }
        for (int i = 0; i < ret[0].length(); i++) {
            String t = "";
            for (int k = 0; k < ret.length; k++)
                t += ret[k].charAt(i);
            String[] s = t.split("-");
            for (int j = 0; j < s.length; j++) {
                if (s[j].length() > 1) {
                    if (wordList.contains(s[j])) {
                        wordList.remove(s[j]);
                        score += scores[s[j].length()];
                    } else {
                        addFatalError("Vertical word " + s[j] + " is not in the dictionary or is duplicated.");
                        return -1;
                    }
                }
            }
        }
        return Math.max(0, score);
    }
}