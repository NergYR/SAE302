public class ClientJava {

    private String cert = "";

    public ClientJava(String cert){
        this.cert = cert;
    }

    public ClientJava(){

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
        
    }

    public void RunSSH(){
        
    }
}
