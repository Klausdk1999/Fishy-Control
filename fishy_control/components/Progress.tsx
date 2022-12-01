import * as React from 'react';
import LinearProgress, { LinearProgressProps } from '@mui/material/LinearProgress';
import Typography from '@mui/material/Typography';
import Box from '@mui/material/Box';

const normalise = (value:number) => ((value - 0) * 100) / (5000 - 0);

function LinearProgressWithLabel(props: any) {
  return (
    <Box sx={{ display: 'flex', alignItems: 'center' }}>
      <Box sx={{ width: '100%', mr: 1 }}>
        <LinearProgress variant="determinate" value={props.value} />
      </Box>
      <Box sx={{ minWidth: 35 }}>
        <Typography variant="body2" color="text.secondary">{`${Math.round(
          props.value,
        )}%`}</Typography>
      </Box>
    </Box>
  );
}

export default function LinearWithValueLabel(props:any) {
  let x = normalise(props.progress)
  console.log (x)
  return (
    <Box sx={{ width: '100%' }}>
      <LinearProgressWithLabel value={x} />
    </Box>
  );
}