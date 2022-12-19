function showVal() {
    if (event.keyCode != 13) return;
    var tmp = document.getElementById("commandline").value;
    document.getElementById("commandline").value = "";
    document.getElementById("console-content").innerHTML += 
        ("<div class=\"entry\">" + tmp + "</div>");
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/console.htm", true);
    xhr.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');
    xhr.onload = function() {
        document.getElementById("console-content").innerHTML +=
            "<div class=\"entry response\">"
            + (JSON.parse(this.responseText).response + "</div>");
    }
    xhr.send(JSON.stringify({command: tmp}));
}