//const name = "Joe"; 


function writeFile()
{
    fs.writeFile("C:\\Users\\snorm\\source\\repos\\More BMP file play\\Dec23\\nodeJSplay\\testText.txt", 
        "Herror? file", err => 
    {
        if (err)
        {
            console.error(err);
        }
    
        else
        {
            console.log("success");
        }
    }
    ); 
}


console.log(__dirname); 
console.log(__filename); 

console.log("Wow");
// console.log(global); 

const fs = require("node:fs"); 

//writeFile(); 





