

public class ClientJava {

    private String cert = "";
    private String addr = "";

    public ClientJava(String cert, String addr){
        this.cert = cert;
        this.addr = addr;
    }

    public ClientJava(String addr){
        this.addr = addr;
    }

    public void Start(){
        if(cert != ""){
            RunSSH();
        }
        else{
            Run();
        }
    }

    public void Run(){
        System.out.println(" 1");
        CommServ cs = new CommServ(this.addr);
        System.out.println(" 2");
        cs.GetInfo();
        System.out.println(" 3");
        cs.SendInfo("RT1FI.csv\n");
        System.out.println(" 4");
        cs.GetInfo();
        System.out.println(" 5");
        cs.Disconnect();
        System.out.println(" 6");
    }

    public void RunSSH(){
        
    }
}
