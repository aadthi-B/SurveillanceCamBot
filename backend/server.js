import express from "express"
import dotenv from "dotenv"
import connectMongoDB from "./db/connectMongoDB.js";
dotenv.config();

const app  = express();

console.log(process.env.MONGO_URI);

const port = process.env.PORT || 3000;

app.listen(port,()=>{
    console.log("Backend server running now...")
    connectMongoDB();
});