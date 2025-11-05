public static class HelloWorld {
    public static void Main(string[] args) {

        System.Console.Out.WriteLine("Hello from C#!");
        if (args.Length > 0) {
            System.Console.Out.WriteLine("Arguments:");
            foreach (var arg in args) {
                System.Console.Out.WriteLine(" - " + arg);
            }
        }
        
    }
}
