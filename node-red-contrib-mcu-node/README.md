
![](/icons/automationNode.png)

# AutomationNode

## This Node-red namespace is for to program routines in a mcuNode

- pinMode: 
    Set the mode (Input/Output) of a given pin of the mcuNode.
- wait: 
    Stop the flow program in a given time in milliseconds.
- digitalWrite: 
    Set the state (High/Low) of a given pin if this one is Output in the pinMode instruction.
- analogWrite: 
    Set the state (0-256) of a given pin if this one is Output in the pinMode instruction.
- digitalRead: 
    Get the state (High/Low) of a given pin if this one is Input in the pinMode instruction.
- analogRead: 
    Get the state (0-4096) of a given pin if this one is Input in the pinMode instruction.

## Blink using sequence using digitalWrite

![](/images/flow1.png)

    [{"id":"c8086a37c0634780","type":"tab","label":"Flow 1","disabled":false,"info":"","env":[]},{"id":"6ab566d281e1b792","type":"junction","z":"c8086a37c0634780","x":260,"y":280,"wires":[["79d725f65dc98519"]]},{"id":"b20fcc4d5035c2e0","type":"junction","z":"c8086a37c0634780","x":300,"y":320,"wires":[["3b8e222f53c4273a"]]},{"id":"b494fc5732f5cc8a","type":"junction","z":"c8086a37c0634780","x":960,"y":320,"wires":[["b20fcc4d5035c2e0"]]},{"id":"8415be043cfda343","type":"junction","z":"c8086a37c0634780","x":180,"y":140,"wires":[["6ab566d281e1b792"]]},{"id":"79d725f65dc98519","type":"pinMode","z":"c8086a37c0634780","name":"pinMode","address":"192.168.0.15","gpio":"13","value":"OUTPUT","x":400,"y":100,"wires":[["bd5c54494ef4ae14"]]},{"id":"bd5c54494ef4ae14","type":"digitalWrite","z":"c8086a37c0634780","name":"digitalWrite","address":"192.168.0.15","gpio":"13","value":"HIGH","x":770,"y":100,"wires":[["711752b843eb0130"]]},{"id":"cd2d537b83fbfbb4","type":"digitalWrite","z":"c8086a37c0634780","name":"digitalWrite","address":"192.168.0.15","gpio":"13","value":"LOW","x":770,"y":280,"wires":[["b494fc5732f5cc8a"]]},{"id":"711752b843eb0130","type":"wait","z":"c8086a37c0634780","name":"wait","timeMs":"10000","x":760,"y":200,"wires":[["cd2d537b83fbfbb4"]]},{"id":"3b8e222f53c4273a","type":"wait","z":"c8086a37c0634780","name":"wait","timeMs":"10000","x":380,"y":200,"wires":[["8415be043cfda343"]]},{"id":"527a20d56033930c","type":"inject","z":"c8086a37c0634780","name":"","props":[{"p":"payload"},{"p":"topic","vt":"str"}],"repeat":"","crontab":"","once":true,"onceDelay":0.1,"topic":"","payload":"","payloadType":"str","x":110,"y":40,"wires":[["b3b417eefa50ae99"]]},{"id":"b3b417eefa50ae99","type":"start","z":"c8086a37c0634780","name":"start","x":130,"y":300,"wires":[["6ab566d281e1b792"]]}]

## Sequence using analogWrite

![](/images/flow2.png)

    [{"id":"71dbcf41cc48b9c5","type":"tab","label":"Flow 2","disabled":false,"info":"","env":[]},{"id":"755afddb634ae0b3","type":"junction","z":"71dbcf41cc48b9c5","x":100,"y":380,"wires":[["73aa5d7e92ccefcb"]]},{"id":"c7f43088ac7ccc60","type":"wait","z":"71dbcf41cc48b9c5","name":"wait","timeMs":"1000","x":660,"y":220,"wires":[["755afddb634ae0b3"]]},{"id":"73aa5d7e92ccefcb","type":"start","z":"71dbcf41cc48b9c5","name":"start","x":230,"y":40,"wires":[["735965308a737f37"]]},{"id":"176db0964d302bf5","type":"inject","z":"71dbcf41cc48b9c5","name":"","props":[{"p":"payload"},{"p":"topic","vt":"str"}],"repeat":"","crontab":"","once":true,"onceDelay":0.1,"topic":"","payload":"","payloadType":"str","x":90,"y":40,"wires":[["73aa5d7e92ccefcb"]]},{"id":"735965308a737f37","type":"analogWrite","z":"71dbcf41cc48b9c5","name":"analogWrite","address":"192.168.0.15","gpio":"2","value":"0","x":380,"y":100,"wires":[["d8db471ecc34192f"]]},{"id":"0df7864b96cdf49a","type":"analogWrite","z":"71dbcf41cc48b9c5","name":"analogWrite","address":"192.168.0.15","gpio":2,"value":"127","x":380,"y":160,"wires":[["b35fe0c30930fd48"]]},{"id":"2646c87adba07ef2","type":"analogWrite","z":"71dbcf41cc48b9c5","name":"analogWrite","address":"192.168.0.15","gpio":2,"value":"255","x":380,"y":220,"wires":[["c7f43088ac7ccc60"]]},{"id":"d8db471ecc34192f","type":"wait","z":"71dbcf41cc48b9c5","name":"wait","timeMs":"1000","x":660,"y":100,"wires":[["0df7864b96cdf49a"]]},{"id":"b35fe0c30930fd48","type":"wait","z":"71dbcf41cc48b9c5","name":"wait","timeMs":"1000","x":660,"y":160,"wires":[["2646c87adba07ef2"]]}]

## Reading the pin state with analogRead and digitalRead

![](/images/flow3.png)

    [{"id":"e5c9ad06546f7836","type":"tab","label":"Flow 3","disabled":false,"info":"","env":[]},{"id":"81d0bef3d5dc94b1","type":"junction","z":"e5c9ad06546f7836","x":100,"y":340,"wires":[["d1bfb86023a1fcd0","9c07e27ae15435d5"]]},{"id":"d93a43c175693492","type":"junction","z":"e5c9ad06546f7836","x":500,"y":340,"wires":[["bf94364a20ce7019"]]},{"id":"d1bfb86023a1fcd0","type":"digitalRead","z":"e5c9ad06546f7836","name":"digitalRead","address":"192.168.0.15","gpio":"34","x":270,"y":320,"wires":[["d93a43c175693492"]]},{"id":"515b50550fc8e1a6","type":"pinMode","z":"e5c9ad06546f7836","name":"pinMode","address":"192.168.0.15","gpio":"34","value":"INPUT","x":270,"y":180,"wires":[["81d0bef3d5dc94b1"]]},{"id":"9c07e27ae15435d5","type":"analogRead","z":"e5c9ad06546f7836","name":"analogRead","address":"192.168.0.15","gpio":"34","x":280,"y":380,"wires":[["d93a43c175693492"]]},{"id":"a94c2e1b2d07a8d1","type":"start","z":"e5c9ad06546f7836","name":"start","x":210,"y":120,"wires":[["515b50550fc8e1a6"]]},{"id":"2bb9f7a43ecf967c","type":"inject","z":"e5c9ad06546f7836","name":"","props":[{"p":"payload"},{"p":"topic","vt":"str"}],"repeat":"","crontab":"","once":true,"onceDelay":0.1,"topic":"","payload":"","payloadType":"str","x":90,"y":120,"wires":[["a94c2e1b2d07a8d1"]]},{"id":"bf94364a20ce7019","type":"debug","z":"e5c9ad06546f7836","name":"","active":true,"tosidebar":true,"console":false,"tostatus":false,"complete":"false","statusVal":"","statusType":"auto","x":610,"y":340,"wires":[]}]


