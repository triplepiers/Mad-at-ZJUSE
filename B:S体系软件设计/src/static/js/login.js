// start 按钮
let start = document.querySelector('.start');
let card = document.querySelector('.card');
start.addEventListener('click', () => {
    start.classList.add('hide');
    card.classList.add('show');
});

let alert_l = document.querySelector('.alert.left')
// 输入检查
function checkSignUp() {
    alert_l.innerHTML = '&nbsp'

    let name = document.querySelector('.signup .name').value
    if(name) {
        if((String)(name).length < 6) {
            alert_l.innerHTML = '用户名长度不足'
            return false
        }
    } else {
        alert_l.innerHTML = '请填写完整信息'
        return false 
    }

    let phone = document.querySelector("#phone").value
    if(phone) {
        if((String)(phone).length != 11) {
            alert_l.innerHTML = '请输入正确的手机号'
            return false
        }
    } else {
        alert_l.innerHTML = '请填写完整信息'
        return false
    }

    let pwd = document.querySelector('.signup .pwd').value
    if(pwd) {
        if((String)(pwd).length < 6) {
            alert_l.innerHTML = '密码长度不足'
            return false
        }
    } else {
        alert_l.innerHTML = '请填写完整信息'
        return false
    }
    
    return true
}

// sign up
let signup = document.querySelector('#signup');
signup.addEventListener('click', () => {
    if(checkSignUp()) {
        let name = document.querySelector('.signup .name').value
        let phone = document.querySelector("#phone").value
        let pwd = document.querySelector('.signup .pwd').value
        $.ajax({
            url: "http://127.0.0.1:8080/user/signup",
            dataType: "json",
            type: "post",
            async: "true",
            data: {
                username: name,
                phone: phone,
                password: pwd
            },
            success: function(res) {
                if (res['status'] == 200) {
                    window.location = "http://127.0.0.1:8080/profile";
                } else if (res['status'] == 501) {
                    document.querySelector('.alert.left').innerHTML = "用户名已存在";
                } else if (res['status'] == 502) {
                    document.querySelector('.alert.left').innerHTML = "手机号已存在";
                }
            }
        })
    }
})

let alert_r = document.querySelector('.alert.right')
// 输入检查
function checkLogIn() {
    let name = document.querySelector('.login .name').value
    let pwd = document.querySelector('.login .pwd').value

    if(!name || !pwd) {
        alert_r.innerHTML = "请填写完整信息"
        return false 
    }
    alert_r.innerHTML = ""
    return true;
}


let login = document.querySelector('#login')
login.addEventListener('click', () => {
    if(checkLogIn()){
        let name = document.querySelector('.login .name').value
        let pwd = document.querySelector('.login .pwd').value
        $.ajax({
            url: "http://127.0.0.1:8080/user/login",
            dataType: "json",
            type: "post",
            async: "true",
            data: {
                username: name,
                password: pwd
            },
            success: function(res) {
                if (res['status'] == 200) {
                    window.location = "http://127.0.0.1:8080/profile";
                } else if (res['status'] == 503) {
                    document.querySelector('.alert.right').innerHTML = "用户不存在";
                } else if (res['status'] == 504) {
                    document.querySelector('.alert.right').innerHTML = "密码错误";
                }
            }
        })
    }
})

// blur 的左右滑动
let blur = document.querySelector('.blur')
let toSign = document.querySelector('.tag.right')

toSign.addEventListener('click', () => {
    blur.classList.add('right')
})

let toLog = document.querySelector('.tag.left')

toLog.addEventListener('click', () => {
    blur.classList.remove('right')
})