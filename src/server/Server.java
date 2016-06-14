package server;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Server side of application
 *
 * @author and pawel
 */
public class Server extends Thread {

    /**
     * server socket
     */
    private ServerSocket serverSocket;
    
    /**
     * socket for each client
     */
    private Socket clientSocket;

    /**
     * creates server socket at the given port
     * @param port port to be open
     * @throws IOException 
     */
    public Server(int port) throws IOException {
        serverSocket = new ServerSocket(port);
    }

    public void run() {
        while (true) {
            try {
                System.out.println("Waiting for client on port "
                        + serverSocket.getLocalPort() + "...");
                clientSocket = serverSocket.accept();
                System.out.println("Just connected to client "
                        + clientSocket.getRemoteSocketAddress());

                while (true) {
                    DataInputStream in
                            = new DataInputStream(clientSocket.getInputStream());
                    String message = in.readUTF();
                    System.out.println(message);
                    switch (message.split(" ")[0]) {

                        case ("levels"):
                            sendString(readFile("levelSelection.txt"));                            
                            break;
                            
                        case ("level"):
                            sendString(readFile(message.split(" ")[1]));
                            break;
//            DataOutputStream out =
//                 new DataOutputStream(clientSocket.getOutputStream());
//            out.writeUTF("Thank you for connecting to "
//              + clientSocket.getLocalSocketAddress() + "\nGoodbye!");
//            clientSocket.close();
                    }
                }
            } catch (SocketTimeoutException s) {
                System.out.println("Socket timed out!");
                break;
            } catch (IOException e) {
                e.printStackTrace();
                break;
            }
        }
    }

    /**
     * method which creates String to send from text file
     *
     * @param filePath path to the text file
     * @return String object to send
     */
    public String readFile(String filePath) {

        String out = "";
        String toSend = "";
        FileInputStream fis;
        try {
            fis = new FileInputStream(filePath);
            InputStreamReader isr = new InputStreamReader(fis);
            BufferedReader br = new BufferedReader(isr);
            
            while((out=br.readLine())!=null){
                toSend += out+" ";
                System.out.println(toSend);
            }
        } catch (FileNotFoundException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }

        return toSend;
    }
    
    /**
     * method which is used to send data to the client
     * @param message data to send
     */
    private void sendString(String message){
      
        try {
            DataOutputStream  out = new DataOutputStream(clientSocket.getOutputStream());
            out.writeUTF(message);
        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }
            
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {

        int portNumber = Integer.parseInt(args[0]);

        try {
            Thread serverThread = new Server(portNumber);
            serverThread.start();

        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }

    }

}
