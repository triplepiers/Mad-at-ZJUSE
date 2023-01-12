// 获取该场景下的所有房间 -> 加载时触发
$(document).ready(function () {
    // 初始化场景信息

    // 添加“新建房间按钮”
    $(".list").append("<div id=\"new_room\" class=\"blue\">+ 新建房间&nbsp&nbsp</div>");
    $("#new_room").click(function(){
        console.log("click")
        let w = document.querySelector("div.msg.f_add_r");
        w.classList.remove("hide");
    })
    $.ajax({
        url: "http://127.0.0.1:8080/room",
        dataType: "json",
        type: "post",
        async: "true",
        data: {},
        success: function(res) {
            var arr = res.data;
            $.each(arr, function(i, v){
                // 添加房间
                html = "<div class=\"room\">" + 
                        "<p>" + v.name + "</p>" +
                        "<ion-icon style=\"--id:" + v.id  + ";\" class=\"arrow\" name=\"chevron-back-circle-outline\"></ion-icon>" +
                        "</div>";
                        // "<div class=\"d_list\">" +
                        // "</div>";
                $(".list").append(html)
                // html = "<div class=\"d_list\">"
                $.each(v.list, function(i, v) {
                    // 分类型处理
                    html = "<div class=\"d_list\">"

                    switch(v.type) {
                        case 1: // light
                            html += "<div class=\"d light\">" +
                                    "<ion-icon name=\"bulb-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "<p class=\"disc\">亮度(%)</p>" +
                                    "<input id=\"i" + v.id+ "\" type=\"text\" oninput =\"if(!/^[0-9]+$/.test(value)) value=value.replace(/\D/g,'');if(value>100)value=100;if(value<0)value=null\" value=\"" + v.info + "\">" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"bulb-outline\"></ion-icon>");
                            break;
                        case 2: // sensor
                            html += "<div class=\"d sensor\">" +
                                    "<ion-icon name=\"radio-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "<p class=\"disc\">上报信息</p>" +
                                    "<p class=\"info\">" + v.info + "</p>" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"radio-outline\"></ion-icon>");
                            break;
                        case 3: // switch
                            html += "<div class=\"d switch\">" +
                                    "<ion-icon name=\"toggle-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"toggle-outline\"></ion-icon>");
                            break;
                        case 4: // lock
                            html += "<div class=\"d lock\">" +
                                    "<ion-icon name=\"lock-closed-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"lock-closed-outline\"></ion-icon>");
                            break;
                    }
                    html += "</div>"
                    $(".list").append(html)
                    // movable
                    var dv = document.getElementById("d"+v.id);
                    dv.style.left = v.x +'px';
                    dv.style.top = v.y + 'px';
                    dv.onmousedown = function(e) {
                        x = e.clientX;
                        y = e.clientY;
                        l = dv.offsetLeft;
                        t = dv.offsetTop;
                        isDown = true;
                        dv.style.cursor = 'move';
            
                        document.onmousemove = function(e) {
                            if (isDown == false) {
                                return;
                            }
                            var nx = e.clientX;
                            var ny = e.clientY;
                            var nl = nx - (x - l);
                            var nt = ny - (y - t);
                            if (nl < 0) {
                                dv.style.left ='0px';
                            } else if (nl > document.body.clientWidth - 85) {
                                dv.style.left = document.body.clientWidth - 85 + 'px';
                            } else {
                                dv.style.left = nl + 'px';
                            }
                            if (nt < 0) {
                                dv.style.top ='0px';
                            } else if (nt > document.body.clientHeight - 100) {
                                dv.style.top = document.body.clientHeight - 100 + 'px';
                            } else {
                                dv.style.top = nt + 'px';
                            }
                        }
            
                        document.onmouseup = function() {
                            isDown = false;
                            dv.style.cursor = 'default';
                        }
                    }
                    if(v.type == 1) {
                        let l = document.getElementById("i"+v.id);
                        l.onchange = function() {
                            $.ajax({
                                url: "http://127.0.0.1:8080/equip/info",
                                dataType: "json",
                                type: "post",
                                async: "true",
                                data: {"eid": v.id, "info": l.value},
                                success: {}
                            })
                        }
                    }

                    if(v.status == 1) {
                        document.getElementById(v.id).classList.add("on");
                    }
                    $("#" + v.id).click(function() {
                        var status = 0;
                        let turn = document.getElementById(v.id)
                        if(turn.classList.contains("on")) {
                            turn.classList.remove("on");
                        } else {
                            turn.classList.add("on");
                            status = 1;
                        }
                        $.ajax({
                            url: "http://127.0.0.1:8080/equip/status",
                            dataType: "json",
                            type: "post",
                            async: "true",
                            data: {"eid": v.id, "status": status},
                            success: {}
                        })
                    })
                    // console.log(v.type)
                })
            })
        }
    })
});

// 增加房间
// 1. 卡片隐藏
$("div.f_add_r ion-icon.close").click(function(){
    let w = document.querySelector("div.f_add_r");
    w.classList.add("hide");
})
// 2. 增加
$("div.f_add_r div.ok").click(function(){
    let name = $("div.f_add_r input").val();
    if(name.length == 0) return; // block empty input
    $("div.f_add_r input").val("") // clear
    // 清空左栏
    $(".list").empty();
    // 添加“新建房间按钮”
    $(".list").append("<div id=\"new_room\" class=\"blue\">+ 新建房间&nbsp&nbsp</div>");
    $("#new_room").click(function(){
        console.log("click")
        let w = document.querySelector("div.msg.f_add_r");
        w.classList.remove("hide");
    })
    $.ajax({
        url: "http://127.0.0.1:8080/room/add",
        dataType: "json",
        type: "post",
        async: "true",
        data: {"name": name, "aid": 1},
        success: function(res) {
            var arr = res.data;
            $.each(arr, function(i, v){
                // 添加房间
                html = "<div class=\"room\">" + 
                        "<p>" + v.name + "</p>" +
                        "<ion-icon style=\"--id:" + v.id  + ";\" class=\"arrow\" name=\"chevron-back-circle-outline\"></ion-icon>" +
                        "</div>";
                        // "<div class=\"d_list\">" +
                        // "</div>";
                $(".list").append(html)
                // html = "<div class=\"d_list\">"
                $.each(v.list, function(i, v) {
                    // 分类型处理
                    html = "<div class=\"d_list\">"

                    switch(v.type) {
                        case 1: // light
                            html += "<div class=\"d light\">" +
                                    "<ion-icon name=\"bulb-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "<p class=\"disc\">亮度(%)</p>" +
                                    "<input id=\"i" + v.id+ "\" type=\"text\" oninput =\"if(!/^[0-9]+$/.test(value)) value=value.replace(/\D/g,'');if(value>100)value=100;if(value<0)value=null\" value=\"" + v.info + "\">" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"bulb-outline\"></ion-icon>");
                            break;
                        case 2: // sensor
                            html += "<div class=\"d sensor\">" +
                                    "<ion-icon name=\"radio-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "<p class=\"disc\">上报信息</p>" +
                                    "<p class=\"info\">" + v.info + "</p>" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"radio-outline\"></ion-icon>");
                            break;
                        case 3: // switch
                            html += "<div class=\"d switch\">" +
                                    "<ion-icon name=\"toggle-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"toggle-outline\"></ion-icon>");
                            break;
                        case 4: // lock
                            html += "<div class=\"d lock\">" +
                                    "<ion-icon name=\"lock-closed-outline\"></ion-icon>" +
                                    "<h2>" + v.name + "</h2>" +
                                    "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                    "</div>";
                            $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"lock-closed-outline\"></ion-icon>");
                            break;
                    }
                    html += "</div>"
                    $(".list").append(html)
                    // movable
                    var dv = document.getElementById("d"+v.id);
                    dv.style.left = v.x +'px';
                    dv.style.top = v.y + 'px';
                    dv.onmousedown = function(e) {
                        x = e.clientX;
                        y = e.clientY;
                        l = dv.offsetLeft;
                        t = dv.offsetTop;
                        isDown = true;
                        dv.style.cursor = 'move';
            
                        document.onmousemove = function(e) {
                            if (isDown == false) {
                                return;
                            }
                            var nx = e.clientX;
                            var ny = e.clientY;
                            var nl = nx - (x - l);
                            var nt = ny - (y - t);
                            if (nl < 0) {
                                dv.style.left ='0px';
                            } else if (nl > document.body.clientWidth - 85) {
                                dv.style.left = document.body.clientWidth - 85 + 'px';
                            } else {
                                dv.style.left = nl + 'px';
                            }
                            if (nt < 0) {
                                dv.style.top ='0px';
                            } else if (nt > document.body.clientHeight - 100) {
                                dv.style.top = document.body.clientHeight - 100 + 'px';
                            } else {
                                dv.style.top = nt + 'px';
                            }
                        }
            
                        document.onmouseup = function() {
                            isDown = false;
                            dv.style.cursor = 'default';
                        }
                    }
                    if(v.type == 1) {
                        let l = document.getElementById("i"+v.id);
                        l.onchange = function() {
                            $.ajax({
                                url: "http://127.0.0.1:8080/equip/info",
                                dataType: "json",
                                type: "post",
                                async: "true",
                                data: {"eid": v.id, "info": l.value},
                                success: {}
                            })
                        }
                    }

                    if(v.status == 1) {
                        document.getElementById(v.id).classList.add("on");
                    }
                    $("#" + v.id).click(function() {
                        var status = 0;
                        let turn = document.getElementById(v.id)
                        if(turn.classList.contains("on")) {
                            turn.classList.remove("on");
                        } else {
                            turn.classList.add("on");
                            status = 1;
                        }
                        $.ajax({
                            url: "http://127.0.0.1:8080/equip/status",
                            dataType: "json",
                            type: "post",
                            async: "true",
                            data: {"eid": v.id, "status": status},
                            success: {}
                        })
                    })
                    // console.log(v.type)
                })
            })
        }
    })
})

// 增加场景
$("#new_scene").click(function() {
    let w = document.querySelector("div.f_add_s");
    w.classList.remove("hide");
})
$("div.f_add_s ion-icon.close").click(function() {
    let w = document.querySelector("div.f_add_s");
    w.classList.add("hide");
})

// 添加设备
// 1. 选项卡显隐
$("#new_device").click(function() {
    (document.querySelector("div.f_add_d")).classList.remove("hide");
})
$("div.f_add_d ion-icon.close").click(function() {
    (document.querySelector("div.f_add_d")).classList.add("hide");
})
// 2. 设备类型选择
let d_types = document.querySelectorAll("div.f_add_d div div.d_type div.btn");
function deactive() {
    d_types.forEach((item) => item.classList.remove("blue"));
    d_types.forEach((item) => item.classList.add("trans"));
    this.classList.add("blue");
}
d_types.forEach((item) => item.addEventListener('click', deactive));
// 3. 发送请求
let add_d_btn = document.getElementById("s_add_d");
add_d_btn.addEventListener('click', () => {
    let d_name = $("#add_d_name").val()
    if(d_name.length==0) return;
    let r_name = $("#add_r_name").val()
    if(r_name.length==0) return;
    // clear
    $("#add_d_name").val("")
    $("#add_r_name").val("")
    // device type
    let d_type = 0;
    for(i = 0 ; i < d_types.length ; i++) {
        if(d_types[i].classList.contains("blue")) {
            d_type = i+1;
            break;
        }
    }
    // send request
    // 清空左栏
    $(".list").empty();
    // 添加“新建房间按钮”
    $(".list").append("<div id=\"new_room\" class=\"blue\">+ 新建房间&nbsp&nbsp</div>");
    $("#new_room").click(function(){
        console.log("click")
        let w = document.querySelector("div.msg.f_add_r");
        w.classList.remove("hide");
    })
    $.ajax({
        url: "http://127.0.0.1:8080/equip/new",
        dataType: "json",
        type: "post",
        async: "true",
        data: {"rname": r_name, "dname": d_name, "dtype": d_type},
        success: function(res) {
            if(res.status == 200) {
                var arr = res.data;
                $.each(arr, function(i, v){
                    // 添加房间
                    html = "<div class=\"room\">" + 
                            "<p>" + v.name + "</p>" +
                            "<ion-icon style=\"--id:" + v.id  + ";\" class=\"arrow\" name=\"chevron-back-circle-outline\"></ion-icon>" +
                            "</div>";
                            // "<div class=\"d_list\">" +
                            // "</div>";
                    $(".list").append(html)
                    // html = "<div class=\"d_list\">"
                    $.each(v.list, function(i, v) {
                        // 分类型处理
                        html = "<div class=\"d_list\">"

                        switch(v.type) {
                            case 1: // light
                                html += "<div class=\"d light\">" +
                                        "<ion-icon name=\"bulb-outline\"></ion-icon>" +
                                        "<h2>" + v.name + "</h2>" +
                                        "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                        "<p class=\"disc\">亮度(%)</p>" +
                                        "<input id=\"i" + v.id+ "\" type=\"text\" oninput =\"if(!/^[0-9]+$/.test(value)) value=value.replace(/\D/g,'');if(value>100)value=100;if(value<0)value=null\" value=\"" + v.info + "\">" +
                                        "</div>";
                                $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"bulb-outline\"></ion-icon>");
                                break;
                            case 2: // sensor
                                html += "<div class=\"d sensor\">" +
                                        "<ion-icon name=\"radio-outline\"></ion-icon>" +
                                        "<h2>" + v.name + "</h2>" +
                                        "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                        "<p class=\"disc\">上报信息</p>" +
                                        "<p class=\"info\">" + v.info + "</p>" +
                                        "</div>";
                                $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"radio-outline\"></ion-icon>");
                                break;
                            case 3: // switch
                                html += "<div class=\"d switch\">" +
                                        "<ion-icon name=\"toggle-outline\"></ion-icon>" +
                                        "<h2>" + v.name + "</h2>" +
                                        "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                        "</div>";
                                $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"toggle-outline\"></ion-icon>");
                                break;
                            case 4: // lock
                                html += "<div class=\"d lock\">" +
                                        "<ion-icon name=\"lock-closed-outline\"></ion-icon>" +
                                        "<h2>" + v.name + "</h2>" +
                                        "<div class=\"turn\" id=\"" + v.id + "\"><div class=\"cir\"></div></div>" +
                                        "</div>";
                                $("div.paint").append("<ion-icon id=\"d" + v.id + "\" name=\"lock-closed-outline\"></ion-icon>");
                                break;
                        }
                        html += "</div>"
                        $(".list").append(html)
                        // movable
                        var dv = document.getElementById("d"+v.id);
                        dv.style.left = v.x +'px';
                        dv.style.top = v.y + 'px';
                        dv.onmousedown = function(e) {
                            x = e.clientX;
                            y = e.clientY;
                            l = dv.offsetLeft;
                            t = dv.offsetTop;
                            isDown = true;
                            dv.style.cursor = 'move';
                
                            document.onmousemove = function(e) {
                                if (isDown == false) {
                                    return;
                                }
                                var nx = e.clientX;
                                var ny = e.clientY;
                                var nl = nx - (x - l);
                                var nt = ny - (y - t);
                                if (nl < 0) {
                                    dv.style.left ='0px';
                                } else if (nl > document.body.clientWidth - 85) {
                                    dv.style.left = document.body.clientWidth - 85 + 'px';
                                } else {
                                    dv.style.left = nl + 'px';
                                }
                                if (nt < 0) {
                                    dv.style.top ='0px';
                                } else if (nt > document.body.clientHeight - 100) {
                                    dv.style.top = document.body.clientHeight - 100 + 'px';
                                } else {
                                    dv.style.top = nt + 'px';
                                }
                            }
                
                            document.onmouseup = function() {
                                isDown = false;
                                dv.style.cursor = 'default';
                            }
                        }
                        if(v.type == 1) {
                            let l = document.getElementById("i"+v.id);
                            l.onchange = function() {
                                $.ajax({
                                    url: "http://127.0.0.1:8080/equip/info",
                                    dataType: "json",
                                    type: "post",
                                    async: "true",
                                    data: {"eid": v.id, "info": l.value},
                                    success: {}
                                })
                            }
                        }

                        if(v.status == 1) {
                            document.getElementById(v.id).classList.add("on");
                        }
                        $("#" + v.id).click(function() {
                            var status = 0;
                            let turn = document.getElementById(v.id)
                            if(turn.classList.contains("on")) {
                                turn.classList.remove("on");
                            } else {
                                turn.classList.add("on");
                                status = 1;
                            }
                            $.ajax({
                                url: "http://127.0.0.1:8080/equip/status",
                                dataType: "json",
                                type: "post",
                                async: "true",
                                data: {"eid": v.id, "status": status},
                                success: {}
                            })
                        })
                        // console.log(v.type)
                    })
                })
            }
        }
    })
})

// 新建场景
let up_file = document.getElementById("upload");
let reader = new FileReader();
up_file.onchange = function() {
    let _file = this.files[0];
    if(_file.type == "image/png") {
        reader.readAsDataURL(_file);
    }
}
$("#btn_add_a").click(function() {
    // console.log("click")
    let area_name = $("#neo_a_name").val()
    if(area_name.length == 0) return;
    $("#neo_a_name").val("");

    img_data = reader.result.substring(22)
    // console.log(img_data)
    $.ajax({
        url: "http://127.0.0.1:8080/area/add",
        dataType: "json",
        type: "post",
        async: "true",
        data: {
            "aname": area_name,
            "img": img_data
        },
        success: function(res) {
            console.log(res)
        }
    })
})

// 退出登陆
$("#exit").click(function(){
    $.ajax({
        url: "http://127.0.0.1:8080/user/logout",
        dataType: "json",
        type: "post",
        async: "true",
        data: {},
        success: function(res) {
            if(res.status == 200) {
                window.location = "http://127.0.0.1:8080/";
            }
        }
    })
})

// 场景前后切换
$("#prev").click(function() {
     // 清空左栏
     $(".list").empty();
     // 添加“新建房间按钮”
     $(".list").append("<div id=\"new_room\" class=\"blue\">+ 新建房间&nbsp&nbsp</div>");
    $.ajax({
        url: "http://127.0.0.1:8080/area/prev",
        dataType: "json",
        type: "post",
        async: "true",
        data: {},
        success: function(res) {
            window.location = "http://127.0.0.1:8080/next";
        }
    })
})
$("#next").click(function() {
     // 清空左栏
     $(".list").empty();
     // 添加“新建房间按钮”
     $(".list").append("<div id=\"new_room\" class=\"blue\">+ 新建房间&nbsp&nbsp</div>");
    $.ajax({
        url: "http://127.0.0.1:8080/area/next",
        dataType: "json",
        type: "post",
        async: "true",
        data: {},
        success: function(res) {
            window.location = "http://127.0.0.1:8080/next";
        }
    })
})

// 保存位置信息
$("#save").click(function () {
    console.log("!save")
    let icons = document.querySelectorAll("div.paint ion-icon");
    var idlist = []
    var xlist = []
    var ylist = []
    for(var i = 1; i < icons.length; i++) { // 第一个是btn，要去掉
        var pos_x = icons[i].offsetLeft;
        var pos_y = icons[i].offsetTop;
        var id = parseInt(icons[i].id.substring(1));
        idlist.push(id);
        xlist.push(pos_x);
        ylist.push(pos_y);
    }
    $.ajax({
        url: "http://127.0.0.1:8080/equip/pos",
        dataType: "json",
        type: "post",
        async: "true",
        data: {"ids": JSON.stringify(idlist), "xs": JSON.stringify(xlist), "ys": JSON.stringify(ylist)},
        success: function(res) {}
    })
})