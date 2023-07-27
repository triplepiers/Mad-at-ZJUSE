package main.util;

public class Dump {

    // 当前工作路径
    private static String workPath = System.getProperty("user.dir");

    public static boolean dumpRemoteDB(String ip) {
        String databaseName="distributed";
        String user="root";
        String pwd="123456";
        String  str1="mysqldump -u" + user +
                " -h" + ip +
                " -P3306 "+
                " -p" + pwd +" "+
                databaseName +" -B "+
                "> " + workPath + "\\sql\\db.sql";
        String str2="mysql -u" + user +
                " -hlocalhost "+
                " -P3306 "+
                " -p" + pwd +" "+
                " -B "+
                "< " + workPath + "\\sql\\db.sql";
        try {
            System.out.println(str1);System.out.println(str2);
            Runtime.getRuntime().exec("cmd /c "+str1);
            Runtime.getRuntime().exec("cmd /c "+str2);
        } catch (Exception e) {
            System.out.println("Error executing command: " + e.getMessage());
        }
        return true;
    }

    public static boolean dumpRemoteTable(String ip, String tableName) {
        String databaseName="try";
        String user="root";
        String pwd="123456";
        String  str1="mysqldump -u" + user +
                " -h" + ip +
                " -P3306 "+
                " -p" + pwd +" "+
                databaseName +" "+tableName+
                "> " + workPath + "\\sql\\" + tableName + ".sql";
        String str2="mysql -u" + user +
                " -hlocalhost "+
                " -P3306 "+
                " -p" + pwd +" "+
                databaseName+" "+tableName+
                "< " + workPath + "\\sql\\" + tableName + ".sql";
        try {
            System.out.println(str1);System.out.println(str2);
            Runtime.getRuntime().exec("cmd /c "+str1);
            Runtime.getRuntime().exec("cmd /c start "+str2);
        } catch (Exception e) {
            System.out.println("Error executing command: " + e.getMessage());
        }
        return true;
    }
}
