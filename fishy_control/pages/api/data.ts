// Next.js API route support: https://nextjs.org/docs/api-routes/introduction
import type { NextApiRequest, NextApiResponse } from 'next';
import type {requestData} from '../index';

let lastReceivedState:requestData = {Temperature:0 , Output:0};

export default function handler(
  req: NextApiRequest,
  res: NextApiResponse  
) {
  const method = req.method;
  lastReceivedState = req.body;
 
  if (method == "GET") {
    
    res.status(200).json(lastReceivedState);
  }
  if (method == "POST") {
    console.log(req.body)
    lastReceivedState = req.body;
    res.send(200);
  }
}

