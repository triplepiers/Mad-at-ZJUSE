package main.util;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

public class RegionMeta {

    public String master = ""; // master 的 connectString

    // 存储的所有 table 名称
    public List<String> tables = new ArrayList<>();

    // 路由相关信息
    public ArrayList<String> slaves = new ArrayList<>(); // 每个 slave 的 connectString
    public int n_slave = 0;
    public int cur_slave = 0; // 下一次 select 操作的 slave 序号

    // 检验 table 是否存在于本 region 中
    public Boolean hasTable(String tName) {
        return this.tables.contains(tName);
    }

    // 是否可写（有主节点 + n_slave >= 2）
    public Boolean isWritable() {
        return (!this.master.equals("") && this.n_slave>1);
    }

    // 是否可读
    public String getReadServer() {
        System.out.println("get read");
        if(this.n_slave>0) {
            System.out.println("has slave");
            this.cur_slave = (this.cur_slave+1) % this.n_slave;
            return this.slaves.get(cur_slave);
        } else if (!this.master.equals("")) {
            return this.master;
        } else {
            return "";
        }
    }
}
