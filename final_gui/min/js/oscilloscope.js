var chartOS=generateOSChart(),displayOSData=!0,x=1,frequency=1,amplitude=1,period=1/frequency,max_volt=0,min_volt=0;function generateOSChart(){var e=document.getElementById("oscilloscopeChart").getContext("2d");return new Chart(e,{type:"line",data:{datasets:[{label:"Voltage",pointStyle:"line",data:[],steppedLine:!1,fill:!1,borderColor:"rgb(14, 173, 105)"}]},options:{plugins:{zoom:{pan:{enabled:!0,mode:"x"},zoom:{enabled:!0,mode:"x"}}},title:{display:!0,text:"Oscilloscope",fontColor:"rgb(255, 255, 255)"},legend:{labels:{defaultFontFamily:"'Roboto', 'Franklin Gothic Medium', 'Tahoma', 'sans-serif",fontColor:"rgb(255, 255, 255)"}},color:"rgb(255, 255, 255)",scales:{yAxes:[{gridLines:{drawOnChartArea:!0,color:"rgb(255,255,255)"},ticks:{fontColor:"rgb(255, 255, 255)",beginAtZero:!0,callback:function(e,t,a){return e+" V"}},scaleLabel:{display:!0,labelString:"Voltage"},type:"linear"}],xAxes:[{gridLines:{drawOnChartArea:!1},ticks:{beginAtZero:!0,fontColor:"rgb(255, 255, 255)",beginAtZero:!0,callback:function(e,t,a){return e}},type:"realtime",scaleLabel:{display:!0,labelString:"Time"},realtime:{onRefresh:function(e){e.data.datasets.forEach((function(e){displayOSData&&e.data.push({x:Date.now(),y:plotSine()})}))},delay:2e3}}]}}})}function updateVars(){frequency=(frequency=parseFloat($("#freq-input").val()))<=0?.01:frequency,amplitude=parseFloat($("#amplitude-input").val())}function plotSine(){return updateStats(),switchX(),$("#max-voltage-input").val(1),$("#min-voltage-input").val(1),x}function switchX(){x=x==amplitude?-1*amplitude:amplitude}function onReceive(e){window.chartOS.config.data.datasets[e.index].data.push({x:e.timestamp,y:e.value}),window.chartOS.update({preservation:!0})}function startOS(){chartOS.clear(),chartOS.options.plugins.streaming.pause=!1,chartOS.update(),displayOSData=!0}function stopOS(){chartOS.options.plugins.streaming.pause=!0,chartOS.update(),displayOSData=!1}