import * as React from 'react';
import { Card } from '@mui/material';
import { Stack } from '@mui/system';
import Menu from '../components/menu'
import useSWR from 'swr';
import CircularIndeterminate from '../components/CircularIndeterminate';
import Progress from '../components/Progress'

export type requestData = {
    Temperature: number,
    Output: number
};

const fetcher = (url:any) => fetch(url).then((res) => res.json());

export default function Dashboard() {

  const { data, error } = useSWR(
    "/api/data",
    fetcher,{ refreshInterval: 10000 }
  );

  if (error) return <div>Failed to load data</div>
  if (!data) return (<Stack direction="column" justifyContent="center" alignItems="center" spacing={3}> <CircularIndeterminate/></Stack>);

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
          {data.Temperature!=0 ? <>{data.Temperature} °C</> : <><CircularIndeterminate/></>} 
        </Card>
        <Card sx={{ fontSize: 25,textAlign: 'center', fontWeight: 700,width: 200, height:70 , p:2 , m:2}}>
          {data.Temperature!=0 ? <>{data.Output}</> : <><CircularIndeterminate/></>}
        </Card>
      </Stack>
      <Stack
      direction="row"
      spacing={3} sx={{color:'black', fontSize: 25,textAlign: 'center', fontWeight: 700, height:70 , p:2 , m:2}}
      >
        Output Power: 
      </Stack>
      <Stack
      direction="row"
      spacing={3}
      sx={{ width: 400, height:70}}
      >
        <Progress sx={{ p:2 , m:2}} progress={data.Output} />
      </Stack>
    </Stack>  
    </>
  );
}