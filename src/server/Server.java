package server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Server side of network application
 *
 * @author Pawel and Marcin
 */
public class Server extends Thread {

    /**
     * server socket
     */
    private final ServerSocket serverSocket;

    /**
     * socket for each client
     */
    private Socket clientSocket;
    
    /**
     * 
     */
    private boolean connectionFlag;

    /**
     * creates server socket at given port
     *
     * @param portNumber port to be open
     * @throws IOException
     */
    public Server(int portNumber) throws IOException {
        serverSocket = new ServerSocket(portNumber);
        connectionFlag = false;
    }

    /**
     * overriden thread method
     * here is described network protocol handling data beetwen client and server
     * 
     * There are couple requests from client, such as:
     * "levels" - requesting for list of available levels
     * "level n" - requesting for certain map configuration of n number
     * "history" - requesting for previous history
     * "save nickname=score" - requesting for saving current game by the pattern written in quotation
     * "logout" -requesting for closing connection
     */
    public void run() {
        while (true) {
            try {
                System.out.println("inet adress ");

                System.out.println("Waiting for client on port "
                        + serverSocket.getLocalPort() + "...");
                clientSocket = serverSocket.accept();
                connectionFlag = true;

                System.out.println("Just connected to"
                        + clientSocket.getRemoteSocketAddress());

                while (connectionFlag) {
                    DataInputStream in;
                    String message;

                    in = new DataInputStream(clientSocket.getInputStream());
                    message = in.readUTF();
                    System.out.println(message);

                    switch (message.split(" ")[0]) {
                        case "levels":
                            sendString(readFile("levelSelection.txt"));
                            break;
                        case "level":
                            sendString(readFile(message.split(" ")[1]));
                            break;
                        case "history":
                            sendString(readFile("GameResults.txt"));
                            break;
                        case "save":
                            saveToFile(message.split(" ")[1]);
                            break;
                        case "logout":
                            sendString("logout successfuly");
                            clientSocket.close();
                            connectionFlag = false;
                            break;
                    }

                }
            } catch (IOException e) {
                e.printStackTrace();
                break;
            }

        }
    }

    /**
     * method which creates string to send from text file
     *
     * @param filePath path to the text file
     * @return string object to send
     */
    private String readFile(String filePath) {
        String out = "", toSent = "";

        try {

            FileInputStream fis = new FileInputStream(filePath);
            InputStreamReader isr = new InputStreamReader(fis);
            BufferedReader br = new BufferedReader(isr);
            while ((out = br.readLine()) != null) {
                toSent += out + " ";
            }
                            System.out.println(toSent);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return toSent;
    }

    /**
     * method which is used to send data to the client
     *
     * @param message data to send
     */
    private void sendString(String message) {
        DataOutputStream out;
        try {
            out = new DataOutputStream(clientSocket.getOutputStream());
            out.writeUTF(message);
        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

        /**
     * method saves player nickname and his score
     *
     * @param data player nickname and his score
     */
    public void saveToFile(String data) {
        try {
            FileWriter file = new FileWriter("GameResults.txt", true);
            BufferedWriter out = new BufferedWriter(file);

            out.write(data + "\r\n");
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        int portNumber = Integer.parseInt(args[0]);

        try {
            Thread t = new Server(portNumber);
            t.start();
        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }

    }

}
