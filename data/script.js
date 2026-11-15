function refreshBatteryPercent() {
    let request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("batteryPercentage").innerHTML = this.responseText;
        }
    }

    request.open("GET", "/refreshBatteryPercent", true);
    request.send();

    refreshBatteryState();
    refreshBatteryLevel();
}

function refreshBatteryState() {
    let request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("batteryState").textContent = this.responseText;
        }
    }

    request.open("GET", "/refreshBatteryState", true);
    request.send();
}

function refreshBatteryLevel() {
    let request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("batteryLevel").textContent = this.responseText;
        }
    }

    request.open("GET", "/refreshBatteryLevel", true);
    request.send();
}


function refreshLfValues() {
    let request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("lfValues").textContent = this.responseText;
        }
    }

    request.open("GET", "/refreshLfValues", true);
    request.send();
}