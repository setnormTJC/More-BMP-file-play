const express = require('express');
const fs = require('fs');
const path = require('path');
const app = express(); 
const port = 3000;




// Define the absolute path to the directory where you want to save the file
const fileDirectory = path.join(__dirname, 'public');

app.use(express.json());
app.use(express.static(fileDirectory));

app.post('/save-coordinates', express.text(), (req, res) =>
{
    const coordinatesText = req.body; 
    const filePath = path.join(fileDirectory, 'clickCoordinates.txt');

    //this is a mechanism to prevent C++ program from trying to READ clickCoordinates.txt while it is being WRITTEN! 
    //"readers/writers problem!"
    // fs.writeFileSync(lockFilePath, 'locky lock'); //creates lock file 

    //fs.appendFile(filePath, )
    fs.writeFile(filePath, coordinatesText, (err) =>
    {
        if (err)
        {
            console.error('Failed to write to file', err); 
            console.log("failed to write to file\n");
            res.status(500).send('Failed to write to file'); 
        }
        else
        {
            // fs.unlinkSync(lockFilePath); 
            res.json({message:'Coordinates saved'});
            console.log("coordinates file written\n");
        }
    });
});

app.listen(port, () =>
{
    console.log(`Server running at http://localhost:${port}`);
})
