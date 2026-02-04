import express from "express";
import { login} from "../controllers/auth.controller.js";
const router = express.Router();
router.post("/username", loginpage);
router.post("/password", loginpage);

export default router;