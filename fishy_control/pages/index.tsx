import * as React from 'react';
import { Card } from '@mui/material';
import { Stack } from '@mui/system';
import axios from 'axios';
import { useState,useEffect } from 'react';
import Menu from '../components/menu'

type requestData = {
    Temperature: number,
    Output: number
};

export default function Dashboard() {

  const [temperature,setTemperature] = useState<number>(0);
  const [output,setOutput] = useState<number>(0);

  useEffect(() => {
    loadData();
    setInterval(loadData, 30000);
    }, []);

  async function loadData() {
    try {
        let { data, status } = await axios.get<requestData>(
            'http://192.168.0.175/temperature',
            {
              headers: {
                Accept: 'application/json',
              },
            },
        );
        setTemperature(data.Temperature);  
       
        setOutput(data.Output);    

        console.log(status);

    } catch (e) {
        console.log(e);
    }
  }

  return (
    <>
    <Menu/>
    <Stack
      direction="column"
      justifyContent="center"
      alignItems="center"
      spacing={3}
    > 
      <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700, width: 400, height:70 , p:2 , m:2}}>
        Dashboard de Controle
      </Card>
      <Stack
      direction="row"
      justifyContent="center"
      alignItems="center"
      spacing={3}
      >
        <Card sx={{ fontSize: 20,textAlign: 'center', fontWeight: 400,width: 200, height:70 , p:2 , m:2}}>
          Termopar tipo K
        </Card>
        <Card sx={{ fontSize: 20,textAlign: 'center', fontWeight: 400,width: 200, height:70 , p:2 , m:2}}>
          PID Output
        </Card>
      </Stack>
      
      <Stack
      direction="row"
      justifyContent="center"
      alignItems="center"
      spacing={3}
      >
        <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700,width: 200, height:70 , p:2 , m:2}}>
          {temperature} C
        </Card>
        <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700,width: 200, height:70 , p:2 , m:2}}>
          {output}
        </Card>
      </Stack>
    </Stack>  
    </>
  );
}