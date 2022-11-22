// Next.js API route support: https://nextjs.org/docs/api-routes/introduction
import type { NextApiRequest, NextApiResponse } from 'next';
import type {requestData} from '../index';

let lastReceivedState:requestData;

export default function handler(
  req: NextApiRequest,
  res: NextApiResponse  
) {
  const method = req.method;
  lastReceivedState = req.body;
 
  if (method == "GET") {
    //teste
    lastReceivedState={Temperature: 50, Output: 100}
    res.status(200).json(lastReceivedState);
  }
  if (method == "POST") {
    lastReceivedState = req.body;


    console.log(lastReceivedState.Temperature+" - "+lastReceivedState.Output);
  }
}

