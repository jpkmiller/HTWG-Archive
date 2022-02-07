package shortestPath;

import graph.AdjacencyListDirectedGraph;
import graph.DirectedGraph;
import sim.SYSimulation;

import java.awt.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.TreeMap;


/**
 * Kürzeste Wege im Scotland-Yard Spielplan mit A* und Dijkstra.
 *
 * @author Oliver Bittel
 * @since 27.02.2019
 */
public class ScotlandYard {

    /**
     * Fabrikmethode zur Erzeugung eines gerichteten Graphens für den Scotland-Yard-Spielplan.
     * <p>
     * Liest die Verbindungsdaten von der Datei ScotlandYard_Kanten.txt.
     * Für die Verbindungen werden folgende Gewichte angenommen:
     * U-Bahn = 5, Taxi = 2 und Bus = 3.
     * Falls Knotenverbindungen unterschiedliche Beförderungsmittel gestatten,
     * wird das billigste Beförderungsmittel gewählt.
     * Bei einer Vebindung von u nach v wird in den gerichteten Graph sowohl
     * eine Kante von u nach v als auch von v nach u eingetragen.
     *
     * @return Gerichteter und Gewichteter Graph für Scotland-Yard.
     * @throws FileNotFoundException
     */
    public static DirectedGraph<Integer> getGraph() throws FileNotFoundException {

        DirectedGraph<Integer> sy_graph = new AdjacencyListDirectedGraph<>();
        Scanner in = new Scanner(new File("ScotlandYard_Kanten.txt"));
        String line;
        while (in.hasNextLine()) {
            line = in.nextLine();
            String[] wf = line.split("\\s+");
            int v1 = Integer.parseInt(wf[0]);
            int v2 = Integer.parseInt(wf[1]);
            double v3;
            switch (wf[2].toLowerCase()) {
                case "ubahn":
                    v3 = 5;
                    break;
                case "taxi":
                    v3 = 2;
                    break;
                case "bus":
                    v3 = 3;
                    break;
                default:
                    throw new UnsupportedOperationException();
            }

            if (sy_graph.containsEdge(v1, v2) && sy_graph.getWeight(v1, v2) < v3)
                v3 = sy_graph.getWeight(v1, v2);
            sy_graph.addEdge(v1, v2, v3);
            sy_graph.addEdge(v2, v1, v3);
        }

        // Test, ob alle Kanten eingelesen wurden:
        System.out.println("Number of Vertices:       " + sy_graph.getNumberOfVertexes());    // 199
        System.out.println("Number of directed Edges: " + sy_graph.getNumberOfEdges());        // 862
        double wSum = 0.0;
        for (Integer v : sy_graph.getVertexSet())
            for (Integer w : sy_graph.getSuccessorVertexSet(v))
                wSum += sy_graph.getWeight(v, w);
        System.out.println("Sum of all Weights:       " + wSum);    // 1972.0

        return sy_graph;
    }


    /**
     * Fabrikmethode zur Erzeugung einer Heuristik für die Schätzung
     * der Distanz zweier Knoten im Scotland-Yard-Spielplan.
     * Die Heuristik wird für A* benötigt.
     * <p>
     * Liest die (x,y)-Koordinaten (Pixelkoordinaten) aller Knoten von der Datei
     * ScotlandYard_Knoten.txt in eine Map ein.
     * Die zurückgelieferte Heuristik-Funktion estimatedCost
     * berechnet einen skalierten Euklidischen Abstand.
     *
     * @return Heuristik für Scotland-Yard.
     * @throws FileNotFoundException
     */
    public static Heuristic<Integer> getHeuristic() throws FileNotFoundException {
        return new ScotlandYardHeuristic();
    }

    /**
     * Scotland-Yard Anwendung.
     *
     * @param args wird nicht verewendet.
     * @throws FileNotFoundException
     */
    public static void main(String[] args) throws FileNotFoundException {

        DirectedGraph<Integer> syGraph = getGraph();

//        Heuristic<Integer> syHeuristic = null; // Dijkstra
        Heuristic<Integer> syHeuristic = getHeuristic(); // A*

        ShortestPath<Integer> sySp = new ShortestPath<Integer>(syGraph, syHeuristic);

        sySp.searchShortestPath(65, 157);
        System.out.println("Distance = " + sySp.getDistance()); // 9.0

        sySp.searchShortestPath(1, 175);
        System.out.println("Distance = " + sySp.getDistance()); // 25.0

        sySp.searchShortestPath(1, 173);
        System.out.println("Distance = " + sySp.getDistance()); // 22.0


        SYSimulation sim;
        try {
            sim = new SYSimulation();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        sySp.setSimulator(sim);
        sim.startSequence("Shortest path from 1 to 173");

        //sySp.searchShortestPath(65,157); // 9.0
        //sySp.searchShortestPath(1,175); //25.0

        sySp.searchShortestPath(1, 173); //22.0
        // bei Heuristik-Faktor von 1/10 wird nicht der optimale Pfad produziert.
        // bei 1/30 funktioniert es.

        System.out.println("Distance = " + sySp.getDistance());
        List<Integer> sp = sySp.getShortestPath();

        int a = -1;
        for (int b : sp) {
            if (a != -1)
                sim.drive(a, b, Color.RED.darker());
            sim.visitStation(b);
            a = b;
        }

        sim.stopSequence();


    }

}

class ScotlandYardHeuristic implements Heuristic<Integer> {
    private Map<Integer, Point> coord; // Ordnet jedem Knoten seine Koordinaten zu

    public ScotlandYardHeuristic() throws FileNotFoundException {
        coord = new TreeMap<>();
        readInCoords();
    }

    private void readInCoords() throws FileNotFoundException {
        Scanner in = new Scanner(new File("ScotlandYard_Knoten.txt"));
        String line;

        while (in.hasNextLine()) {
            line = in.nextLine();
            String[] wf = line.split("([\\t|\\s])+");
            coord.put(Integer.parseInt(wf[0]), new Point(Integer.parseInt(wf[1]), Integer.parseInt(wf[2])));
        }
    }

    public double estimatedCost(Integer u, Integer v) {
        Point p1 = coord.get(u);
        Point p2 = coord.get(v);

        double factor = 1.0/30.0;

        return Math.sqrt(Math.pow(p1.x - p2.x, 2) + Math.pow(p1.y - p2.y, 2)) * factor;
    }

    private static class Point {
        int x;
        int y;

        Point(int x, int y) {
            this.x = x;
            this.y = y;
        }
    }
}

