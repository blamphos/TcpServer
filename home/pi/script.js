var audio_icon = document.getElementById("audio");
var mute_icon = document.getElementById("mute");
var coax1_btn = document.getElementById("coax1");
var coax2_btn = document.getElementById("coax2");
var opt1_btn = document.getElementById("opt1");
var auto_btn = document.getElementById("auto");

var slider = document.getElementById("myRange");

function toggleMute() {
	slider.value = 0;
	onVolumeChanged();
}

audio_icon.addEventListener("click", toggleMute, false);
mute_icon.addEventListener("click", toggleMute, false);

function onVolumeChanged() {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			var arr = this.responseText.split(";");
			if (arr.length == 2) {
				var level = Number(arr[0]);
				var input = Number(arr[1]);
				slider.value = level;
				audio_icon.style.visibility = (level == 0) ? "hidden" : "visible";
				mute_icon.style.visibility = (level == 0) ? "visible" : "hidden";
			}
		}
	};
	xhttp.open("POST", "pot=" + slider.value, true);
	xhttp.send();
}

function onInputChanged(input) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			var arr = this.responseText.split(";");
			if (arr.length == 2) {
				var input = Number(arr[1]);
				coax1_btn.className = (input == 0) ? "btn1 btn1" : "btn1 btn2";
				coax2_btn.className = (input == 1) ? "btn1 btn1" : "btn1 btn2";
				opt1_btn.className = (input == 2) ? "btn1 btn1" : "btn1 btn2";
				auto_btn.className = (input == 3) ? "btn1 btn1" : "btn1 btn2";

			}
		}
	};
	xhttp.open("POST", "spdif=" + input, true);
	xhttp.send();
}

function openNav() {
  document.getElementById("mySidepanel").style.width = "250px";
}

function closeNav() {
  document.getElementById("mySidepanel").style.width = "0";
}

var prevClick = new Date();

function onVolumeUpdate(direction) {
  var now = new Date();
  var change = (now - prevClick < 500) ? 3 : 1;
  prevClick = now;
  
  for (var i = 0; i < change; i++) {
	  direction > 0 ? slider.value++ : slider.value--;
  }
  
  if (slider.value < 0) {
	  slider.value = 0;
  }
  else if (slider.value > 99) {
	  slider.value = 99;
  }

  //document.getElementById("level").innerHTML = slider.value;
  onVolumeChanged();
}
