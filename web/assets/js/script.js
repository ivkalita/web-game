function WebSocketTest() {
	if ("WebSocket" in window) {
    	var ws = new WebSocket("ws://localhost:1337/ws")

    	ws.onopen = function() {
        	ws.send("Hello, world!");
    	};

    	ws.onmessage = function(evt) { 
        	var msg = evt.data;
        	alert("Message received: " + msg);
        	ws.close();
      	};

    	ws.onclose = function() { 
        	alert("WebSocket closed.");
    	};
  	} else {
    	alert("This browser does not support WebSockets.");
  	}
}
